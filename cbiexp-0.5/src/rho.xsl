<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


  <!-- main master template for generated page -->
  <xsl:template match="/">
    <html>
      <head>
	<title>Correlation Matrix For Top Predictors</title>
      </head>

      <body>
	<xsl:copy-of select="*"/>
      </body>
    </html>
  </xsl:template>


</xsl:stylesheet>
