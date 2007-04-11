function printFeatureSlice(filename, I)
   doc = com.mathworks.xml.XMLUtils.createDocument('features');
   docRoot = doc.getDocumentElement();
   xmlify(doc, docRoot, {num2cell(I)}, 'feature', {'index'});
   xmlwrite(filename, doc);
