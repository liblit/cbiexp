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

  <xsl:variable name="preds" select="document('predictor-info.xml')/predictor-info/info"/>
  <xsl:variable name="features" select="document('features.xml')/featureinfos/info"/>

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
            <xsl:call-template name="featurefooter" mode="feature"/>
          </table>
        </div>
      </body>
    </html> 
  </xsl:template>

  <xsl:template match="featureclaimed" mode="feature">
    <xsl:variable name="index" select="@index"/>
    <xsl:variable name="feature" select="$features[number($index)]"/>
    <xsl:variable name="aspectindex" select="../@index"/>
    <xsl:variable name="aspectentry" select="$feature/aspectprob[number($aspectindex)]"/>
    <tr>
      <th><xsl:value-of select="$index"/></th>
      <xsl:apply-templates select="$aspectentry"/>
      <xsl:apply-templates select="$preds[number($index)]" mode="static-cells"/>
    </tr>
  </xsl:template>

  <xsl:template match="aspectprob" mode="feature">
    <td><xsl:value-of select="@prob"/></td>
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

  <xsl:template name="featurefooter" mode="feature">
    <tfoot>
      <tr> 
        <th>Totals</th>
        <td><xsl:value-of select="@ratio"/></td>
      </tr>
    </tfoot>
  </xsl:template>

</xsl:stylesheet>
