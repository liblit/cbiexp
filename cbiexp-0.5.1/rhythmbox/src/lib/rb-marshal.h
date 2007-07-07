
#ifndef __rb_marshal_MARSHAL_H__
#define __rb_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,INT,INT,BOXED,UINT,UINT (./rb-marshal.list:1) */
extern void rb_marshal_VOID__OBJECT_INT_INT_BOXED_UINT_UINT (GClosure     *closure,
                                                             GValue       *return_value,
                                                             guint         n_param_values,
                                                             const GValue *param_values,
                                                             gpointer      invocation_hint,
                                                             gpointer      marshal_data);

/* VOID:OBJECT,INT,INT (./rb-marshal.list:2) */
extern void rb_marshal_VOID__OBJECT_INT_INT (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* VOID:STRING,INT (./rb-marshal.list:3) */
extern void rb_marshal_VOID__STRING_INT (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

/* VOID:STRING,STRING (./rb-marshal.list:4) */
extern void rb_marshal_VOID__STRING_STRING (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* VOID:STRING,STRING,STRING (./rb-marshal.list:5) */
extern void rb_marshal_VOID__STRING_STRING_STRING (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);

/* BOOLEAN:POINTER (./rb-marshal.list:6) */
extern void rb_marshal_BOOLEAN__POINTER (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

/* VOID:DOUBLE (./rb-marshal.list:7) */
#define rb_marshal_VOID__DOUBLE	g_cclosure_marshal_VOID__DOUBLE

/* VOID:POINTER,INT,POINTER (./rb-marshal.list:8) */
extern void rb_marshal_VOID__POINTER_INT_POINTER (GClosure     *closure,
                                                  GValue       *return_value,
                                                  guint         n_param_values,
                                                  const GValue *param_values,
                                                  gpointer      invocation_hint,
                                                  gpointer      marshal_data);

G_END_DECLS

#endif /* __rb_marshal_MARSHAL_H__ */

