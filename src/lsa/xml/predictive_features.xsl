<!DOCTYPE stylesheet [
  <!ENTITY link "&amp;#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

<xsl:import href="logo.xsl"/>
<xsl:import href="scores.xsl"/>

<xsl:output method="html"/>

<xsl:param name="index"/>

<xsl:variable name="kind" select="document('aspects.xml',/)/aspectinfos/aspect[position() = $index]/@kind"/>
<xsl:variable name="preds" select="document('predictor-info.xml')/predictor-info/info"/>
<xsl:variable name="source-dir" select="document('summary.xml',/)/summary/@source-dir"/>
<xsl:variable name="mypreds" select="document('predictive_features.xml',/)/predictivefeatures/aspect[position() = $index]/feature"/>

  <xsl:template match="/">
    <xsl:variable name="title">
      <xsl:text>Aspect </xsl:text>
      <xsl:value-of select="$index"/>
      <xsl:text> (</xsl:text>
      <xsl:value-of select="$kind"/>
      <xsl:text> )</xsl:text>
    </xsl:variable>  
    <xsl:variable name="imagetitle">
      <xsl:text>aspect_</xsl:text>
      <xsl:value-of select="$index"/>
      <xsl:text>_predictive_features.png</xsl:text>
    </xsl:variable>
    <html>
      <body>
        <div id="frontmatter">
          <h1><xsl:copy-of select="$logo-icon"/><xsl:value-of select="$title"/></h1>
        </div>
        <div id="rest">
          <img src="{$imagetitle}"/> 
          <table>
            <xsl:call-template name="featureheader"/>
            <tbody>
              <xsl:apply-templates select="$mypreds"/>  
            </tbody>
          </table>
        </div>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="feature">
    <xsl:variable name="index" select="@index"/>
    <tr>
      <th>
        <xsl:value-of select="position()"/>
      </th>
      <td>
        <xsl:value-of select="@index"/>
      </td>
      <td>
        <xsl:value-of select="@score"/>
      </td>
      <xsl:apply-templates select="$preds[position() = $index]" mode="static-cells">
        <xsl:with-param name="source-dir" select="$source-dir"/>
      </xsl:apply-templates>
    </tr>
  </xsl:template>

  <xsl:template name="featureheader">
    <thead>
      <tr>
        <th>Rank</th>
        <th>Index</th>
        <th>Score</th>
        <th>Predicate</th>
        <th>Function</th>
        <th>File:Line</th>
      </tr>
    </thead>
  </xsl:template>

</xsl:stylesheet>
