<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="operands.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


  <!-- main master template for generated page -->
  <xsl:template match="/">
    <html>
      <head>
	<title>Logistic Regression Analysis</title>
	<link rel="stylesheet" href="view.css" type="text/css"/>
      </head>

      <body>
	<xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>


  <!-- table of predictors -->
  <xsl:template match="logreg">
    <table class="predictors">
      <thead>
	<tr>
	  <td>Importance</td>
	  <td>Predicate</td>
	  <td>Function</td>
	  <td>File:Line</td>
	</tr>
      </thead>
      <tfoot/>
      <tbody>
	<xsl:apply-templates/>
      </tbody>
    </table>
  </xsl:template>


  <!-- one predictor row -->
  <xsl:template match="predictor">
    <tr>
      <td><xsl:value-of select="@importance"/></td>
      <td><xsl:apply-templates mode="operands" select="."/></td>
      <td><xsl:value-of select="@function"/></td>
      <td><xsl:value-of select="@file"/>:<xsl:value-of select="@line"/></td>
      </tr>
  </xsl:template>


</xsl:stylesheet>
