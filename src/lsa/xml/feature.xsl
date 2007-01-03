<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <xsl:import href="logo.xsl"/>
  <xsl:import href="scores.xsl"/>

  <xsl:variable name="features" select="document('predictor-info.xml')/predictor-info/info"/>

  <xsl:output method="html"/>

  <xsl:template name="feature">
    <xsl:param name="num"/>
    <xsl:apply-templates select="/plsa/aspect[@index = $num]" mode="feature"/>
  </xsl:template>

  <xsl:template match="aspect" mode="feature">
    <xsl:variable name="title">Aspect <xsl:value-of select="@index"/> Features</xsl:variable>
    <html>
      <head>
        <title><xsl:value-of select="$title"/></title>
      </head>
      <body>
        <div id="frontmatter"> 
          <h1><xsl:copy-of select="$logo-icon"/><xsl:value-of select="$title"/></h1>
        </div>
        <div id="rest">
          <table>
            <xsl:call-template name="featureheader" mode="feature"/>
            <tbody>
              <xsl:apply-templates select="featureclaimed" mode="feature"/>
            </tbody> 
          </table>
        </div>
      </body>
    </html> 
  </xsl:template>

  <xsl:template match="featureclaimed" mode="feature">
    <xsl:variable name="feature" select="id(@idref)"/>
    <xsl:variable name="index" select="$feature/@infoindex"/>
    <tr>
      <th><xsl:value-of select="$index"/></th>
      <td><xsl:value-of select="@probability"/></td>
      <xsl:apply-templates select="$features[number($index)]" mode="static-cells"/> 
    </tr>
  </xsl:template>

  <xsl:template name="featureheader" mode="feature">
    <thead>
      <tr>
        <th>Index</th>
        <th>Probability</th>
        <th>Predicate</th>
        <th>Function</th>
        <th>File:Line</th>
      </tr>
    </thead>
  </xsl:template>

</xsl:stylesheet>
