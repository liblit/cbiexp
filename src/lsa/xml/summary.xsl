<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

<xsl:import href="logo.xsl"/>

<xsl:output method="html"/>

  <xsl:template match="/">
    <xsl:variable name="title">pLSA Results</xsl:variable>  
    <html>
      <body>
        <div id="frontmatter">
          <h1><xsl:copy-of select="$logo-icon"/><xsl:value-of select="$title"/></h1>
        </div>
        <div id="rest">
          <xsl:call-template name="summary"/>
          <xsl:call-template name="features"/>
          <xsl:call-template name="aspects"/>
        </div>
      </body>
    </html>
  </xsl:template>

  <xsl:template name="summary">
    <div id="summary">
      <h2>Summary:</h2>
      <xsl:call-template name="runs"/>
    </div>
  </xsl:template>

  <xsl:template name="runs">
    <xsl:variable name="runs" select="document('runs.xml')/runinfos/run"/>
    <div id="runs">
      <h3>Runs:</h3>
      <table>
        <thead>
          <th></th>
          <th>Count</th>
        </thead>
        <tbody>
          <tr>
            <th style="text-align:left">Successes</th>
            <xsl:call-template name="rungroup">
              <xsl:with-param name="group" select="$runs[@outcome='success']"/>
            </xsl:call-template>
          </tr>
          <tr>
            <th style="text-align:left">Failures</th>
            <xsl:call-template name="rungroup">
              <xsl:with-param name="group" select="$runs[@outcome='failure']"/>
            </xsl:call-template>
          </tr>
          <tr>
            <th style="text-align:left">Discards</th>
            <xsl:call-template name="rungroup">
              <xsl:with-param name="group" select="$runs[@outcome='ignore']"/>
            </xsl:call-template>
          </tr>
        </tbody>
        <tfoot>
          <tr>
            <th style="text-align:left">Total</th>
            <xsl:call-template name="rungroup">
              <xsl:with-param name="group" select="$runs"/>
            </xsl:call-template>
          </tr>
        </tfoot>
      </table>
    </div>
  </xsl:template>

  <xsl:template name="rungroup">
    <xsl:param name="group"/>
    <xsl:variable name="count" select="count($group)"/>
    <td style="text-align:right"><xsl:value-of select="$count"/></td>
  </xsl:template>

  <xsl:template name="features">
    <xsl:variable name="features" select="document('features.xml')/featureinfos/feature"/>
    <div id="features"> 
      <h3>Features:</h3>
      <table>
        <tr>
          <th>Total Features:</th>
          <td><xsl:value-of select="count($features)"/></td>
        </tr>
      </table>
    </div>
  </xsl:template>

  <xsl:template name="aspects">
    <xsl:variable name="aspects" select="document('aspects.xml')/aspectinfos/aspect"/>
    <div class="aspects">
    <h2>Aspects:</h2>
    <img src="./claimed_runs.png" align="left"/> 
    <table class="aspecttable">
      <xsl:call-template name="aspectheader"/>
      <tbody>
      <xsl:for-each select="$aspects">
        <xsl:call-template name="aspectrow"/>
      </xsl:for-each>
      </tbody>
    </table>
    </div>
  </xsl:template>

  <xsl:template name="aspectheader">
    <thead>
    <tr>
      <th>Index</th>
      <th>Kind</th>
      <th>Detail</th>
    </tr>
    </thead>
  </xsl:template>

  <xsl:template name="aspectrow">
    <xsl:variable name="linkurl">
        <xsl:text>./aspect</xsl:text><xsl:value-of select="position()"/><xsl:text>.html</xsl:text>
    </xsl:variable>
    <tr>
      <th><xsl:value-of select="position()"/></th>
      <td style="text-align:center"><xsl:value-of select="@kind"/></td>
      <td style="text-align:center"><a href="{$linkurl}">&link;</a></td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
