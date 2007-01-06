<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:cbi="http://www.cs.wisc.edu/cbi"
>

  <xsl:import href="logo.xsl"/>
  <xsl:import href="scores.xsl"/>

  <xsl:variable name="preds" select="document('predictor-info.xml')/predictor-info/info"/>

  <xsl:output method="html"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="cbi:aspectfeatures">
    <xsl:variable name="title">Aspect <xsl:value-of select="cbi:aspect/@index"/> Features</xsl:variable>
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
            <xsl:call-template name="featureheader"/>
            <tbody>
              <xsl:apply-templates select="cbi:feature"/>
            </tbody> 
          </table>
        </div>
      </body>
    </html> 
  </xsl:template>

  <xsl:template match="cbi:feature">
    <xsl:variable name="index" select="@index"/>
    <tr>
      <th><xsl:value-of select="@index"/></th>
      <td><xsl:value-of select="@probability"/></td>
      <td><xsl:value-of select="@cumulative"/></td>
      <xsl:apply-templates select="$preds[number($index)]" mode="static-cells"/>
    </tr>
  </xsl:template>

  <xsl:template name="featureheader">
    <thead>
      <tr>
        <th>Index</th>
        <th>Probability</th>
        <th>Cumulative</th>
        <th>Predicate</th>
        <th>Function</th>
        <th>File:Line</th>
      </tr>
    </thead>
  </xsl:template>

</xsl:stylesheet>
