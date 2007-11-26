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
<xsl:variable name="myruns" select="document('predictive_runs.xml',/)/predictiveruns/aspect[position() = $index]/run"/> 
<xsl:variable name="theruns" select="document('runs.xml',/)/runinfos/run"/>

  <xsl:template match="/">
    <xsl:variable name="title">
      <xsl:text>Aspect </xsl:text>
      <xsl:value-of select="$index"/>
      <xsl:text> (</xsl:text>
      <xsl:value-of select="$kind"/>
      <xsl:text> ): Predictive Runs</xsl:text>
    </xsl:variable>  
    <xsl:variable name="imagetitle">
      <xsl:text>aspect_</xsl:text>
      <xsl:value-of select="$index"/>
      <xsl:text>_predictive_runs.png</xsl:text>
    </xsl:variable>
    <html>
      <body>
        <div id="frontmatter">
          <h1><xsl:copy-of select="$logo-icon"/><xsl:value-of select="$title"/></h1>
        </div>
        <div id="rest">
          <img src="{$imagetitle}"/> 
          <table>
            <xsl:call-template name="runheader"/>
            <tbody>
              <xsl:apply-templates select="$myruns"/>  
            </tbody>
          </table>
        </div>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="run">
    <xsl:variable name="index" select="@index"/>
    <xsl:variable name="thisrun" select="$theruns[position() = $index]"/>
    <xsl:variable name="rundir" select="$thisrun/@runid"/>
    <xsl:variable name="count" select="$thisrun/@sum"/>
    <tr>
      <th>
        <xsl:value-of select="position()"/>
      </th>
      <td style="text-align:right">
        <xsl:value-of select="@index"/>
      </td>
      <td style="text-align:right">
        <xsl:value-of select="@score"/>
      </td>
      <td style="text-align:right">
        <xsl:value-of select="$rundir"/>
      </td>
      <td style="text-align:right">
        <xsl:value-of select="$count"/>
      </td>
    </tr>
  </xsl:template>

  <xsl:template name="runheader">
    <thead>
      <tr>
        <th>Rank</th>
        <th>Index</th>
        <th>Score</th>
        <th>Run</th> 
        <th>Count</th>
      </tr>
    </thead>
  </xsl:template>

</xsl:stylesheet>
