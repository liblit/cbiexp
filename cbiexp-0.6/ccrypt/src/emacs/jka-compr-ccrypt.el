;;; jka-compr-ccrypt.el --- reading/writing/loading compressed/encrypted files

;; Copyright (C) 1993, 1994, 1995, 1997  Free Software Foundation, Inc.
;; Copyright (C) 2001 Peter Selinger

;; Author: jka@ece.cmu.edu (Jay K. Adams)
;; Changes: selinger@users.sourceforge.net (Peter Selinger)
;; Maintainer: Peter Selinger
;; Keywords: data

;; This is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; This software is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this software; see the file COPYING.  If not, write to the
;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;;; Commentary: 

;; This package implements low-level support for reading, writing,
;; and loading compressed or encrypted files.  It hooks into the low-level file
;; I/O functions (including write-region and insert-file-contents) so
;; that they automatically compress or uncompress a file if the file
;; appears to need it (based on the extension of the file name).
;; Packages like Rmail, VM, GNUS, and Info should be able to work
;; with compressed files without modification.

;;; Commentary on ccrypt support:

;; support for encryption/decryption with ccrypt was added by Peter
;; Selinger.  We have to deal with prompting users for passwords,
;; remembering passwords for each buffer, etc.

;; limitations:

;; On some systems, passing the password to ccrypt in an environment
;; variable may not be safe; this happens if users are able to display the
;; environment of processes they don't own with the 'ps' program. I don't
;; know of any system where this is still the case, but I heard that they
;; may exist. In any case, ccrypt deletes the value from its environment as
;; soon as it has been read; so this should not be a problem in most
;; situations.

;; Sometimes emacs will choke if it wants to auto-save a buffer in the
;; absense of a password or a minibuffer for prompting for it. I have not
;; yet figured out how to handle this situation; however, it does not arise
;; during "normal" operation because when a buffer's name has the ".cpt"
;; extension, a password is normally always defined for that buffer.

;; Under certain circumstances, unencrypted data will be written to disk
;; temporarily. For instance, during each encryption operation, the
;; unencrypted data is written to a temporary file. This is because the
;; call-process built-in function is designed to read from a file and write
;; to a buffer, not the other way around. I don't know a way around this
;; problem at the moment. Another potential way in which unencrypted data
;; might appear on disk is if buffer contents are swapped out from main
;; memory.

;; auto-save files should not be a problem, since such files are also
;; encrypted if the buffer's filename indicates that they should be so.

;;; todo:

;; fix bug where "sh" does not return proper return value.
;; add support for .gz.cpt and .gzc

;;; done:

;; pass password to ccrypt in environment variable not on the command line
;; add meaningful prompt: "Password for /tmp/test.cpt: "
;; blind password input ********
;; on failed password, continue re-asking for password.
;; generate a better way of patterning passwords.
;; make acceptable-retval-list local to each program called
;; add encrypt/decrypt mode with password
;; make password local to buffer
;; on read operation, prompt for password
;; on write operation, use buffer's password
;; on insert operation, do not change buffer's password
;; on failed password, do not remember wrong password, prompt next time

;; INSTRUCTIONS:
;;
;; To use jka-compr-ccrypt, simply load this package, and edit as usual.
;; One way to do this automatically is to include the lines
;;  (setq load-path (cons "<path>" load-path))
;;  (require 'jka-compr-ccrypt "jka-compr-ccrypt.el")
;; in your .emacs file, where <path> is the pathname where this file 
;; is found.
;;
;; The operation of this package should be transparent to the user (except
;; for messages appearing when a file is being compressed or uncompressed,
;; encrypted or decrypted).
;;
;; The variable, jka-compr-ccrypt-compression-info-list can be used to
;; customize jka-compr-ccrypt to work with other compression programs.
;; The default value of this variable allows jka-compr-ccrypt to work with
;; Unix compress and gzip.
;;
;; If you don't want messages about compressing or encryption
;; to show up in the echo area, you can set the compress-name and
;; decompress-name fields of the jka-compr-ccrypt-compression-info-list to
;; nil.
;;
;; The password for a buffer can be changed with the interactive command
;; M-x ccrypt-set-buffer-password. Note that the change does not affect
;; anything until the next time the buffer is saved.

;; ACKNOWLEDGMENTS
;; 
;; jka-compr-ccrypt is an adaptation of and a replacement for
;; jka-compr, which is part of GNU Emacs.
;;
;; jka-compr is a V19 adaptation of jka-compr for V18 of Emacs.  Many people
;; have made helpful suggestions, reported bugs, and even fixed bugs in 
;; jka-compr.  I recall the following people as being particularly helpful.
;;
;;   Jean-loup Gailly
;;   David Hughes
;;   Richard Pieri
;;   Daniel Quinlan
;;   Chris P. Ross
;;   Rick Sladkey
;;
;; Andy Norman's ange-ftp was the inspiration for the original jka-compr for
;; Version 18 of Emacs.
;;
;; After I had made progress on the original jka-compr for V18, I learned of a
;; package written by Kazushi Jam Marukawa, called jam-zcat, that did exactly
;; what I was trying to do.  I looked over the jam-zcat source code and
;; probably got some ideas from it.
;;


;;; Code:

(defgroup compression nil
  "Data compression utilities"
  :group 'data)

(defgroup jka-compr-ccrypt nil
  "jka-compr-ccrypt customization"
  :group 'compression)


(defcustom jka-compr-ccrypt-shell "sh"
  "*Shell to be used for calling compression programs. This is only used to
discard part of the output when a file is partially uncompressed.  Note:
the hard-coded syntax in jka-compr-ccrypt more or less assumes that this is
either sh or bash. See also the function jka-compr-ccrypt-shell-escape."
  :type 'string
  :group 'jka-compr-ccrypt)

(defun jka-compr-ccrypt-shell-escape (x)
  "Takes a string and returns its escaped form to be used on the
command line of the shell whose name is set in jka-compr-ccrypt-shell."
  (concat "\"" 
	  (apply 'concat 
		 (mapcar (function 
			  (lambda (c)
			    (if (memq c '(?\" ?\\ ?\$))
				(list ?\\ c)
			      (char-to-string c))))
			 x)) 
	  "\""))


;;; I have this defined so that .Z files are assumed to be in unix compress
;;; format; .tgz and .gz files in gzip format, and .bz2 files in bzip fmt,
;;; and .cpt file in ccrypt format.
(defcustom jka-compr-ccrypt-compression-info-list
  ;;[regexp
  ;; compr-message  compr-prog  compr-args
  ;; uncomp-message uncomp-prog uncomp-args
  ;; can-append auto-mode-flag retval-list password-flag]
  '(["\\.Z\\(~\\|\\.~[0-9]+~\\)?\\'"
     "compressing"    ("compress" "-c")
     "uncompressing"  ("uncompress" "-c")
     nil t (0) nil]
     ;; Formerly, these had an additional arg "-c", but that fails with
     ;; "Version 0.1pl2, 29-Aug-97." (RedHat 5.1 GNU/Linux) and
     ;; "Version 0.9.0b, 9-Sept-98".
    ["\\.bz2\\(~\\|\\.~[0-9]+~\\)?\\'"
     "bzip2ing"       ("bzip2")
     "bunzip2ing"     ("bzip2" "-d")
     nil t (0) nil]
    ["\\.tgz\\'"
     "zipping"        ("gzip" "-c" "-q")
     "unzipping"      ("gzip" "-c" "-q" "-d")
     t nil (0 2) nil]
    ["\\.gz\\(~\\|\\.~[0-9]+~\\)?\\'"
     "zipping"        ("gzip" "-c" "-q")
     "unzipping"      ("gzip" "-c" "-q" "-d")
     t t (0 2) nil]
    ["\\.cpt\\(\\#\\|~\\|\\.~[0-9]+~\\)?\\'"
     "encrypting"     ("ccrypt" "-q" "-E" "KEY")
     "decrypting"     ("ccrypt" "-q" "-d" "-E" "KEY")
     nil t (0) t])

  "List of vectors that describe available compression and encryption
techniques.  Each element, which describes a compression or encryption
technique, is a vector of the form [REGEXP COMPRESS-MSG COMPRESS-COMMAND
UNCOMPRESS-MSG UNCOMPRESS-COMMAND APPEND-FLAG AUTO-MODE-FLAG RETVAL-LIST
PASSWORD-FLAG], where:

   regexp                is a regexp that matches filenames that are
                         compressed with this format

   compress-msg          is the message to issue to the user when doing this
                         type of compression (nil means no message)

   compress-command      is a command that performs this compression, that
                         is, a list consisting of a program name and arguments

   uncompress-msg        is the message to issue to the user when doing this
                         type of uncompression (nil means no message)

   uncompress-command    is a command that performs this compression, that
                         is, a list consisting of a program name and arguments

   append-flag           is non-nil if this compression technique can be
                         appended

   auto-mode-flag        non-nil means strip the regexp from file names
                         before attempting to set the mode

   retval-list           list of acceptable return values for compress
                         and uncompress program

   password-flag         non-nil if we are dealing with encryption rather
                         than compression. In this case, the password is
                         passed to the ccrypt command in the environment 
                         variable KEY."

  :type '(repeat (vector regexp
			 (choice :tag "Compress Message"
				 (string :format "%v")
				 (const :tag "No Message" nil))
			 (repeat :tag "Compress Command" string)
			 (choice :tag "Uncompress Message"
				 (string :format "%v")
				 (const :tag "No Message" nil))
			 (repeat :tag "Uncompress Command" string)
			 (boolean :tag "Append")
			 (boolean :tag "Auto Mode")
                         (repeat :tag "Acceptable Return Values" integer)
                         (boolean :tag "Password Mode")))
  :group 'jka-compr-ccrypt)

(defvar jka-compr-ccrypt-mode-alist-additions
  (list (cons "\\.tgz\\'" 'tar-mode))
  "A list of pairs to add to `auto-mode-alist' when jka-compr-ccrypt is installed.")

;; List of all the elements we actually added to file-coding-system-alist.
(defvar jka-compr-ccrypt-added-to-file-coding-system-alist nil)

(defvar jka-compr-ccrypt-file-name-handler-entry
  nil
  "The entry in `file-name-handler-alist' used by the jka-compr-ccrypt I/O functions.")

;;; Functions for accessing the return value of jka-compr-ccrypt-get-compression-info
(defun jka-compr-ccrypt-info-regexp               (info)  (aref info 0))
(defun jka-compr-ccrypt-info-compress-message     (info)  (aref info 1))
(defun jka-compr-ccrypt-info-compress-command     (info)  (aref info 2))
(defun jka-compr-ccrypt-info-uncompress-message   (info)  (aref info 3))
(defun jka-compr-ccrypt-info-uncompress-command   (info)  (aref info 4))
(defun jka-compr-ccrypt-info-can-append           (info)  (aref info 5))
(defun jka-compr-ccrypt-info-strip-extension      (info)  (aref info 6))
(defun jka-compr-ccrypt-info-retval-list          (info)  (aref info 7))
(defun jka-compr-ccrypt-info-password-flag        (info)  (aref info 8))

(defun jka-compr-ccrypt-get-compression-info (filename)
  "Return information about the compression scheme of FILENAME.
The determination as to which compression scheme, if any, to use is
based on the filename itself and `jka-compr-ccrypt-compression-info-list'."
  (catch 'compression-info
    (let ((case-fold-search nil))
      (mapcar
       (function (lambda (x)
		   (and (string-match (jka-compr-ccrypt-info-regexp x) filename)
			(throw 'compression-info x))))
       jka-compr-ccrypt-compression-info-list)
      nil)))

(defun jka-compr-ccrypt-substitute (list key value)
  "Replace key by value in list"
  (mapcar (function (lambda (x) 
		      (if (eq x key)
			  value
			x)))
	  list))

(defvar jka-compr-ccrypt-buffer-password nil
  "The encryption password. This variable is buffer-local.")

(make-variable-buffer-local 'jka-compr-ccrypt-buffer-password)

(defun jka-compr-ccrypt-read-passwd (&optional confirm default)
  (read-passwd (format "Password for %s: " (buffer-name)) confirm default))

(defun jka-compr-ccrypt-get-buffer-password (&optional buffer)
  "Get encryption password for BUFFER (default: current buffer). 
Return nil if not set."
  (with-current-buffer (or buffer (current-buffer))
    jka-compr-ccrypt-buffer-password))

(defun jka-compr-ccrypt-set-buffer-password (password &optional buffer)
  "Set the encryption password for BUFFER (default: current buffer)."
  (with-current-buffer (or buffer (current-buffer))
    (setq jka-compr-ccrypt-buffer-password password)))

(defun ccrypt-set-buffer-password ()
  "Set the encryption password for current buffer."
  (interactive "")
  (setq jka-compr-ccrypt-buffer-password 
	(jka-compr-ccrypt-read-passwd t)))

(put 'compression-error 'error-conditions '(compression-error file-error error))

;(defvar jka-compr-ccrypt-acceptable-retval-list '(0 2 141))

(defun jka-compr-ccrypt-error (command infile message &optional errfile)

  (let ((errbuf (get-buffer-create " *jka-compr-ccrypt-error*"))
	(curbuf (current-buffer)))
    (with-current-buffer errbuf
      (widen) (erase-buffer)
      (insert (format "Error while executing \"%s < %s\"\n\n"
		      (mapconcat 'identity command " ")
		      infile))
      
      (and errfile
	   (insert-file-contents errfile)))
     (display-buffer errbuf))

  (signal 'compression-error
	  (list "Opening input file" (format "error %s" message) infile)))

(defvar jka-compr-ccrypt-dd-program
  "/bin/dd")

(defvar jka-compr-ccrypt-dd-blocksize 256)

(defun jka-compr-ccrypt-partial-uncompress (command message infile beg
					     len retvals &optional password)
  "Call program PROG with ARGS args taking input from INFILE.
Fourth and fifth args, BEG and LEN, specify which part of the output
to keep: LEN chars starting BEG chars from the beginning.
Sixth arg, RETVALS, specifies acceptable return values.
Seventh arg, &optional PASSWORD, specifies encryption password, if any."
  (let* ((skip (/ beg jka-compr-ccrypt-dd-blocksize))
	 (prefix (- beg (* skip jka-compr-ccrypt-dd-blocksize)))
	 (count (and len (1+ (/ (+ len prefix) jka-compr-ccrypt-dd-blocksize))))
	 (start (point))
	 (dd (format "%s bs=%d skip=%d %s 2> /dev/null"
		     jka-compr-ccrypt-dd-program
		     jka-compr-ccrypt-dd-blocksize
		     skip
		     ;; dd seems to be unreliable about
		     ;; providing the last block.  So, always
		     ;; read one more than you think you need.
		     (if count (concat "count=" (1+ count)) "")))
	 (pipe-command (append command (list "|" dd))))
    
    (setq password (jka-compr-ccrypt-call-process pipe-command
						  jka-compr-ccrypt-shell 
						  message infile t retvals 
						  password))

    ;; Delete the stuff after what we want, if there is any.
    (and
     len
     (< (+ start prefix len) (point))
     (delete-region (+ start prefix len) (point)))

    ;; Delete the stuff before what we want.
    (delete-region start (+ start prefix)))
  password)

(defun jka-compr-ccrypt-call-process2 (command infile buffer display &optional shell)
  "Similar to call-process. If SHELL is given and non-nil, then execute
the given command in the given shell. COMMAND in this case is a list
of strings, which are concatenated (with spaces) before execution.
Redirections, pipelines, etc, are permissible. If SHELL is absent or
nil, then execute the command directly, without a shell. In this case,
command must be a list of a program name, followed by individual
command line arguments."
  
  (if shell
      (call-process shell infile buffer display 
		    "-c" (mapconcat 'identity command " "))
    (apply 'call-process (car command) infile buffer display (cdr command)))
  )

(defun jka-compr-ccrypt-call-process (command shell message infile output retvals &optional password)

  (let ((err-file (jka-compr-ccrypt-make-temp-name))
	(coding-system-for-read (or coding-system-for-read 'undecided))
	(coding-system-for-write 'no-conversion)
	(buffer output)
	done)
    
    (unwind-protect
	
	(while (not done)
	  (if password
	      (setenv "KEY" password))
	  (let*
	      ((status
		(if shell
		    (call-process shell infile (list buffer err-file) nil 
				  "-c" (mapconcat 'identity command " "))
		  (apply 'call-process (car command) infile 
			 (list buffer err-file) nil (cdr command)))
		))
	      
	    (cond ((and password (eq status 4))
		   (message "Bad password; please try again")
		   (sit-for 1)
		   (setq password (jka-compr-ccrypt-read-passwd)))
		  ((not (memq status retvals))
		   (jka-compr-ccrypt-error command
				    infile message err-file))
		  (t
		   (setq done t)))))
	  
      (jka-compr-ccrypt-delete-temp-file err-file))
    
    password))
 

;;; Support for temp files.  Much of this was inspired if not lifted
;;; from ange-ftp.

(defcustom jka-compr-ccrypt-temp-name-template
  (expand-file-name "jka-com" temporary-file-directory)
  "Prefix added to all temp files created by jka-compr-ccrypt.
There should be no more than seven characters after the final `/'."
  :type 'string
  :group 'jka-compr-ccrypt)

(defvar jka-compr-ccrypt-temp-name-table (make-vector 31 nil))

(defun jka-compr-ccrypt-make-temp-name (&optional local-copy)
  "This routine will return the name of a new file."
  (let* ((lastchar ?a)
	 (prevchar ?a)
	 (template (concat jka-compr-ccrypt-temp-name-template "aa"))
	 (lastpos (1- (length template)))
	 (not-done t)
	 file
	 entry)

    (while not-done
      (aset template lastpos lastchar)
      (setq file (concat (make-temp-name template) "#"))
      (setq entry (intern file jka-compr-ccrypt-temp-name-table))
      (if (or (get entry 'active)
	      (file-exists-p file))

	  (progn
	    (setq lastchar (1+ lastchar))
	    (if (> lastchar ?z)
		(progn
		  (setq prevchar (1+ prevchar))
		  (setq lastchar ?a)
		  (if (> prevchar ?z)
		      (error "Can't allocate temp file.")
		    (aset template (1- lastpos) prevchar)))))

	(put entry 'active (not local-copy))
	(setq not-done nil)))

    file))


(defun jka-compr-ccrypt-delete-temp-file (temp)

  (put (intern temp jka-compr-ccrypt-temp-name-table)
       'active nil)

  (condition-case ()
      (delete-file temp)
    (error nil)))


(defun jka-compr-ccrypt-write-region (start end file &optional append visit)
  (let* ((filename (expand-file-name file))
	 (visit-file (if (stringp visit) (expand-file-name visit) filename))
	 (info (jka-compr-ccrypt-get-compression-info visit-file)))
      
      (if info

	  (let ((can-append (jka-compr-ccrypt-info-can-append info))
		(compress-message (jka-compr-ccrypt-info-compress-message info))
		(compress-command (jka-compr-ccrypt-info-compress-command info))
		(password (if (jka-compr-ccrypt-info-password-flag info) 
			      (or (jka-compr-ccrypt-get-buffer-password) 
				  (jka-compr-ccrypt-read-passwd t))
			    nil))
		(retvals (jka-compr-ccrypt-info-retval-list info))
		(base-name (file-name-nondirectory visit-file))
		temp-file temp-buffer
		;; we need to leave `last-coding-system-used' set to its
		;; value after calling write-region the first time, so
		;; that `basic-save-buffer' sees the right value.
		(coding-system-used last-coding-system-used))

	    (setq temp-buffer (get-buffer-create " *jka-compr-ccrypt-wr-temp*"))
	    (with-current-buffer temp-buffer
	      (widen) (erase-buffer))

	    (if (and append
		     (not can-append)
		     (file-exists-p filename))
		
		(let* ((local-copy (file-local-copy filename))
		       (local-file (or local-copy filename)))
		  
		  (setq temp-file local-file))

	      (setq temp-file (jka-compr-ccrypt-make-temp-name)))

	    (and 
	     compress-message
	     (message "%s %s..." compress-message base-name))
	    
	    (jka-compr-ccrypt-run-real-handler 'write-region
					(list start end temp-file t 'dont))
	    ;; save value used by the real write-region
	    (setq coding-system-used last-coding-system-used)

	    ;; Here we must read the output of compress program as is
	    ;; without any code conversion.
	    (let ((coding-system-for-read 'no-conversion))
	      (setq password 
		    (jka-compr-ccrypt-call-process compress-command
					    nil
					    (concat compress-message
						    " " base-name)
					    temp-file
					    temp-buffer
					    retvals
					    password)))

	    (with-current-buffer temp-buffer
              (let ((coding-system-for-write 'no-conversion))
                (if (memq system-type '(ms-dos windows-nt))
                    (setq buffer-file-type t) )
                (jka-compr-ccrypt-run-real-handler 'write-region
                                            (list (point-min) (point-max)
                                                  filename
                                                  (and append can-append) 'dont))
                (erase-buffer)) )

	    (jka-compr-ccrypt-delete-temp-file temp-file)

	    (and
	     compress-message
	     (message "%s %s...done" compress-message base-name))

	    (cond
	     ((eq visit t)
	      (setq buffer-file-name filename)
	      (jka-compr-ccrypt-set-buffer-password password)
	      (set-visited-file-modtime))
	     ((stringp visit)
	      (setq buffer-file-name visit)
	      (jka-compr-ccrypt-set-buffer-password password)
	      (let ((buffer-file-name filename))
		(set-visited-file-modtime))))

	    (and (or (eq visit t)
		     (eq visit nil)
		     (stringp visit))
		 (message "Wrote %s" visit-file))

	    ;; ensure `last-coding-system-used' has an appropriate value
	    (setq last-coding-system-used coding-system-used)

	    nil)
	      
	(jka-compr-ccrypt-run-real-handler 'write-region
				    (list start end filename append visit)))))


(defun jka-compr-ccrypt-insert-file-contents (file &optional visit beg end replace)
  (barf-if-buffer-read-only)

  (and (or beg end)
       visit
       (error "Attempt to visit less than an entire file"))

  (let* ((filename (expand-file-name file))
	 (info (jka-compr-ccrypt-get-compression-info filename)))

    (if info

	(let ((uncompress-message (jka-compr-ccrypt-info-uncompress-message info))
	      (uncompress-command (jka-compr-ccrypt-info-uncompress-command info))
	      (password (if (jka-compr-ccrypt-info-password-flag info) 
			    (jka-compr-ccrypt-read-passwd) nil))
	      (retvals (jka-compr-ccrypt-info-retval-list info))
	      (base-name (file-name-nondirectory filename))
	      (notfound nil)
	      (local-copy
	       (jka-compr-ccrypt-run-real-handler 'file-local-copy (list filename)))
	      local-file
	      size start
              (coding-system-for-read
	       (or coding-system-for-read
		   ;; If multibyte characters are disabled,
		   ;; don't do that conversion.
		   (and (null enable-multibyte-characters)
			(or (auto-coding-alist-lookup
			     (jka-compr-ccrypt-byte-compiler-base-file-name file))
			    'raw-text))
		   (let ((coding (find-operation-coding-system
				  'insert-file-contents
				  (jka-compr-ccrypt-byte-compiler-base-file-name file))))
		     (and (consp coding) (car coding)))
		   'undecided)) )

	  (setq local-file (or local-copy filename))

	  (if visit
	      (setq buffer-file-name filename))

	  (unwind-protect		; to make sure local-copy gets deleted

	      (progn
		  
		(and
		 uncompress-message
		 (message "%s %s..." uncompress-message base-name))

		(condition-case error-code

		    (progn
		      (if replace
			  (goto-char (point-min)))
		      (setq start (point))
		      (if (or beg end)
			  (jka-compr-ccrypt-partial-uncompress 
			   uncompress-command
			   (concat uncompress-message
				   " " base-name)
			   local-file
			   (or beg 0)
			   (if (and beg end)
			       (- end beg)
			     end)
			   retvals
			   password)
			;; If visiting, bind off buffer-file-name so that
			;; file-locking will not ask whether we should
			;; really edit the buffer.
			(let ((buffer-file-name
			       (if visit nil buffer-file-name)))
			  (setq password
				(jka-compr-ccrypt-call-process 
				 uncompress-command
				 nil
				 (concat uncompress-message
					 " " base-name)
				 local-file
				 t
				 retvals
				 password))))
		      (setq size (- (point) start))
		      (if replace
			  (let* ((del-beg (point))
				 (del-end (+ del-beg size)))
			    (delete-region del-beg
					   (min del-end (point-max)))))
		      (goto-char start))
		  (error
		   (if (and (eq (car error-code) 'file-error)
			    (eq (nth 3 error-code) local-file))
		       (if visit
			   (setq notfound error-code)
			 (signal 'file-error 
				 (cons "Opening input file"
				       (nthcdr 2 error-code))))
		     (signal (car error-code) (cdr error-code))))))

	    (and
	     local-copy
	     (file-exists-p local-copy)
	     (delete-file local-copy)))

	  (and
	   visit
	   (progn
	     (unlock-buffer)
	     (setq buffer-file-name filename)
	     (set-visited-file-modtime)))

	  (and visit
	     (jka-compr-ccrypt-set-buffer-password password))
	    
	  (and
	   uncompress-message
	   (message "%s %s...done" uncompress-message base-name))

	  (and
	   visit
	   notfound
	   (signal 'file-error
		   (cons "Opening input file" (nth 2 notfound))))

	  ;; This is done in insert-file-contents after we return.
	  ;; That is a little weird, but better to go along with it now
	  ;; than to change it now.

;;;	  ;; Run the functions that insert-file-contents would.
;;; 	  (let ((p after-insert-file-functions)
;;; 		(insval size))
;;; 	    (while p
;;; 	      (setq insval (funcall (car p) size))
;;; 	      (if insval
;;; 		  (progn
;;; 		    (or (integerp insval)
;;; 			(signal 'wrong-type-argument
;;; 				(list 'integerp insval)))
;;; 		    (setq size insval)))
;;; 	      (setq p (cdr p))))

	  (list filename size))

      (jka-compr-ccrypt-run-real-handler 'insert-file-contents
				  (list file visit beg end replace)))))


(defun jka-compr-ccrypt-file-local-copy (file)
  (let* ((filename (expand-file-name file))
	 (info (jka-compr-ccrypt-get-compression-info filename)))

    (if info

	(let* ((uncompress-message (jka-compr-ccrypt-info-uncompress-message info))
	      (uncompress-command (jka-compr-ccrypt-info-uncompress-command info))
	      (password (if (jka-compr-ccrypt-info-password-flag info) 
			    (jka-compr-ccrypt-read-passwd) nil))
	      (retvals (jka-compr-ccrypt-info-retval-list info))
	      (base-name (file-name-nondirectory filename))
	      (local-copy
	       (jka-compr-ccrypt-run-real-handler 'file-local-copy (list filename)))
	      (temp-file (jka-compr-ccrypt-make-temp-name t))
	      (temp-buffer (get-buffer-create " *jka-compr-ccrypt-flc-temp*"))
	      (notfound nil)
	      local-file)

	  (setq local-file (or local-copy filename))

	  (unwind-protect

	      (with-current-buffer temp-buffer
		  
		(and
		 uncompress-message
		 (message "%s %s..." uncompress-message base-name))
		  
		;; Here we must read the output of uncompress program
		;; and write it to TEMP-FILE without any code
		;; conversion.  An appropriate code conversion (if
		;; necessary) is done by the later I/O operation
		;; (e.g. load).
		(let ((coding-system-for-read 'no-conversion)
		      (coding-system-for-write 'no-conversion))

		  (jka-compr-ccrypt-call-process 
		   uncompress-command
		   (concat uncompress-message
			   " " base-name)
		   local-file
		   t
		   retvals
		   password)

		  (and
		   uncompress-message
		   (message "%s %s...done" uncompress-message base-name))

		  (write-region
		   (point-min) (point-max) temp-file nil 'dont)))

	    (and
	     local-copy
	     (file-exists-p local-copy)
	     (delete-file local-copy))

	    (kill-buffer temp-buffer))

	  temp-file)
	    
      (jka-compr-ccrypt-run-real-handler 'file-local-copy (list filename)))))


;;; Support for loading compressed files.
(defun jka-compr-ccrypt-load (file &optional noerror nomessage nosuffix)
  "Documented as original."

  (let* ((local-copy (jka-compr-ccrypt-file-local-copy file))
	 (load-file (or local-copy file)))

    (unwind-protect

	(let (inhibit-file-name-operation
	      inhibit-file-name-handlers)
	  (or nomessage
	      (message "Loading %s..." file))

	  (let ((load-force-doc-strings t))
	    (load load-file noerror t t))

	  (or nomessage
	      (message "Loading %s...done." file)))

      (jka-compr-ccrypt-delete-temp-file local-copy))

    t))

(defun jka-compr-ccrypt-byte-compiler-base-file-name (file)
  (let ((info (jka-compr-ccrypt-get-compression-info file)))
    (if (and info (jka-compr-ccrypt-info-strip-extension info))
	(save-match-data
	  (substring file 0 (string-match (jka-compr-ccrypt-info-regexp info) file)))
      file)))

(put 'write-region 'jka-compr-ccrypt 'jka-compr-ccrypt-write-region)
(put 'insert-file-contents 'jka-compr-ccrypt 'jka-compr-ccrypt-insert-file-contents)
(put 'file-local-copy 'jka-compr-ccrypt 'jka-compr-ccrypt-file-local-copy)
(put 'load 'jka-compr-ccrypt 'jka-compr-ccrypt-load)
(put 'byte-compiler-base-file-name 'jka-compr-ccrypt
     'jka-compr-ccrypt-byte-compiler-base-file-name)

(defvar jka-compr-ccrypt-inhibit nil
  "Non-nil means inhibit automatic uncompression temporarily.
Lisp programs can bind this to t to do that.
It is not recommended to set this variable permanently to anything but nil.")

(defun jka-compr-ccrypt-handler (operation &rest args)
  (save-match-data
    (let ((jka-op (get operation 'jka-compr-ccrypt)))
      (if (and jka-op (not jka-compr-ccrypt-inhibit))
	  (apply jka-op args)
	(jka-compr-ccrypt-run-real-handler operation args)))))

;; If we are given an operation that we don't handle,
;; call the Emacs primitive for that operation,
;; and manipulate the inhibit variables
;; to prevent the primitive from calling our handler again.
(defun jka-compr-ccrypt-run-real-handler (operation args)
  (let ((inhibit-file-name-handlers
	 (cons 'jka-compr-ccrypt-handler
	       (and (eq inhibit-file-name-operation operation)
		    inhibit-file-name-handlers)))
	(inhibit-file-name-operation operation))
    (apply operation args)))

;;;###autoload(defun auto-compression-mode (&optional arg)
;;;###autoload  "\
;;;###autoloadToggle automatic file compression and uncompression.
;;;###autoloadWith prefix argument ARG, turn auto compression on if positive, else off.
;;;###autoloadReturns the new status of auto compression (non-nil means on)."
;;;###autoload  (interactive "P")
;;;###autoload  (if (not (fboundp 'jka-compr-ccrypt-installed-p))
;;;###autoload      (progn
;;;###autoload        (require 'jka-compr-ccrypt)
;;;###autoload        ;; That turned the mode on, so make it initially off.
;;;###autoload        (toggle-auto-compression)))
;;;###autoload  (toggle-auto-compression arg t))

(defun toggle-auto-compression (&optional arg message)
  "Toggle automatic file compression and uncompression.
With prefix argument ARG, turn auto compression on if positive, else off.
Returns the new status of auto compression (non-nil means on).
If the argument MESSAGE is non-nil, it means to print a message
saying whether the mode is now on or off."
  (interactive "P\np")
  (let* ((installed (jka-compr-ccrypt-installed-p))
	 (flag (if (null arg)
		   (not installed)
		 (or (eq arg t) (listp arg) (and (integerp arg) (> arg 0))))))

    (cond
     ((and flag installed) t)		; already installed

     ((and (not flag) (not installed)) nil) ; already not installed

     (flag
      (jka-compr-ccrypt-install))

     (t
      (jka-compr-ccrypt-uninstall)))


    (and message
	 (if flag
	     (message "Automatic file (de)compression is now ON.")
	   (message "Automatic file (de)compression is now OFF.")))

    flag))

(defun jka-compr-ccrypt-build-file-regexp ()
  (concat
   "\\("
   (mapconcat
    'jka-compr-ccrypt-info-regexp
    jka-compr-ccrypt-compression-info-list
    "\\)\\|\\(")
   "\\)"))


(defun jka-compr-ccrypt-install ()
  "Install jka-compr-ccrypt.
This adds entries to `file-name-handler-alist' and `auto-mode-alist'
and `inhibit-first-line-modes-suffixes'."

  (setq jka-compr-ccrypt-file-name-handler-entry
	(cons (jka-compr-ccrypt-build-file-regexp) 'jka-compr-ccrypt-handler))

  (setq file-name-handler-alist (cons jka-compr-ccrypt-file-name-handler-entry
				      file-name-handler-alist))

  (setq jka-compr-ccrypt-added-to-file-coding-system-alist nil)

  (mapcar
   (function (lambda (x)
	       ;; Don't do multibyte encoding on the compressed files.
	       (let ((elt (cons (jka-compr-ccrypt-info-regexp x)
				 '(no-conversion . no-conversion))))
		 (setq file-coding-system-alist
		       (cons elt file-coding-system-alist))
		 (setq jka-compr-ccrypt-added-to-file-coding-system-alist
		       (cons elt jka-compr-ccrypt-added-to-file-coding-system-alist)))

	       (and (jka-compr-ccrypt-info-strip-extension x)
		    ;; Make entries in auto-mode-alist so that modes
		    ;; are chosen right according to the file names
		    ;; sans `.gz'.
		    (setq auto-mode-alist
			  (cons (list (jka-compr-ccrypt-info-regexp x)
				      nil 'jka-compr-ccrypt)
				auto-mode-alist))
		    ;; Also add these regexps to
		    ;; inhibit-first-line-modes-suffixes, so that a
		    ;; -*- line in the first file of a compressed tar
		    ;; file doesn't override tar-mode.
		    (setq inhibit-first-line-modes-suffixes
			  (cons (jka-compr-ccrypt-info-regexp x)
				inhibit-first-line-modes-suffixes)))))
   jka-compr-ccrypt-compression-info-list)
  (setq auto-mode-alist
	(append auto-mode-alist jka-compr-ccrypt-mode-alist-additions)))


(defun jka-compr-ccrypt-uninstall ()
  "Uninstall jka-compr-ccrypt.
This removes the entries in `file-name-handler-alist' and `auto-mode-alist'
and `inhibit-first-line-modes-suffixes' that were added
by `jka-compr-ccrypt-installed'."
  ;; Delete from inhibit-first-line-modes-suffixes
  ;; what jka-compr-ccrypt-install added.
  (mapcar
     (function (lambda (x)
		 (and (jka-compr-ccrypt-info-strip-extension x)
		      (setq inhibit-first-line-modes-suffixes
			    (delete (jka-compr-ccrypt-info-regexp x)
				    inhibit-first-line-modes-suffixes)))))
     jka-compr-ccrypt-compression-info-list)

  (let* ((fnha (cons nil file-name-handler-alist))
	 (last fnha))

    (while (cdr last)
      (if (eq (cdr (car (cdr last))) 'jka-compr-ccrypt-handler)
	  (setcdr last (cdr (cdr last)))
	(setq last (cdr last))))

    (setq file-name-handler-alist (cdr fnha)))

  (let* ((ama (cons nil auto-mode-alist))
	 (last ama)
	 entry)

    (while (cdr last)
      (setq entry (car (cdr last)))
      (if (or (member entry jka-compr-ccrypt-mode-alist-additions)
	      (and (consp (cdr entry))
		   (eq (nth 2 entry) 'jka-compr-ccrypt)))
	  (setcdr last (cdr (cdr last)))
	(setq last (cdr last))))
    
    (setq auto-mode-alist (cdr ama)))

  (let* ((ama (cons nil file-coding-system-alist))
	 (last ama)
	 entry)

    (while (cdr last)
      (setq entry (car (cdr last)))
      (if (member entry jka-compr-ccrypt-added-to-file-coding-system-alist)
	  (setcdr last (cdr (cdr last)))
	(setq last (cdr last))))
    
    (setq file-coding-system-alist (cdr ama))))

      
(defun jka-compr-ccrypt-installed-p ()
  "Return non-nil if jka-compr-ccrypt is installed.
The return value is the entry in `file-name-handler-alist' for jka-compr-ccrypt."

  (let ((fnha file-name-handler-alist)
	(installed nil))

    (while (and fnha (not installed))
     (and (eq (cdr (car fnha)) 'jka-compr-ccrypt-handler)
	   (setq installed (car fnha)))
      (setq fnha (cdr fnha)))

    installed))


;;; Add the file I/O hook if it does not already exist.
;;; Make sure that jka-compr-ccrypt-file-name-handler-entry is eq to the
;;; entry for jka-compr-ccrypt in file-name-handler-alist.
(and (jka-compr-ccrypt-installed-p)
     (jka-compr-ccrypt-uninstall))

(jka-compr-ccrypt-install)


(provide 'jka-compr-ccrypt)

;; jka-compr-ccrypt.el ends here.
