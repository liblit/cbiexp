<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:exsl="http://exslt.org/common"
  extension-element-prefixes="exsl"
>

<xsl:import href="logo.xsl"/>
<xsl:import href="rawfeature.xsl"/>
<xsl:import href="feature.xsl"/>

<xsl:output method="html"/>

  <xsl:template match="/">
    <xsl:variable name="title">pLSA Results</xsl:variable>  
    <html>
      <head>
        <title><xsl:value-of select="$title"/></title>
      </head>
      <body>
        <div id="frontmatter">
            <h1><xsl:copy-of select="$logo-icon"/><xsl:value-of select="$title"/></h1>
        </div>
        <div id="rest">
          <xsl:call-template name="summary"/>
          <xsl:call-template name="aspects"/>
          <xsl:call-template name="failingruns"/>  
        </div>
      </body>
    </html>
  </xsl:template>

  <xsl:template name="summary">
    <div id="summary">
      <h2>Summary:</h2>
      <xsl:call-template name="runs"/>
      <xsl:call-template name="features"/>
    </div>
  </xsl:template>

  <xsl:template name="runs">
    <xsl:variable name="runs" select="/plsa/run"/>
    <div id="runs">
      <h3>Runs:</h3>
      <table>
        <thead>
          <th></th>
          <th>Count</th>
          <th>Mean Length</th>
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
    <xsl:variable name="total" select="sum($group/@totalcount)"/>
    <xsl:variable name="mean">
      <xsl:choose>
        <xsl:when test="$total=0">
          <xsl:value-of select="number(0)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="round($total div $count)"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <td style="text-align:right"><xsl:value-of select="$mean"/></td>
  </xsl:template>

  <xsl:template name="features">
    <xsl:variable name="features" select="document('predictor-info.xml')/predictor-info/info"/>
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
    <div class="aspects">
    <h2>Aspects:</h2>
    <h3>Usage:</h3>
    <table class="aspecttable">
      <xsl:call-template name="aspectheader"/>
      <tbody>
      <xsl:for-each select="plsa/aspect[@kind='usage']">
        <xsl:call-template name="aspectrow"/>
      </xsl:for-each>
      </tbody>
      <xsl:call-template name="aspectfooter">
        <xsl:with-param name="runs" select="id(plsa/aspect[@kind='usage']/runid/@idref)"/>
      </xsl:call-template>
    </table>
    <h3>Buggy:</h3>
    <table class="aspecttable">
      <xsl:call-template name="aspectheader"/>
      <tbody>
      <xsl:for-each select="plsa/aspect[@kind='bug']">
        <xsl:call-template name="aspectrow"/>
      </xsl:for-each>
      </tbody>
      <xsl:call-template name="aspectfooter">
        <xsl:with-param name="runs" select="id(plsa/aspect[@kind='bug']/runid/@idref)"/>
      </xsl:call-template>
    </table>
    </div>
  </xsl:template>

  <xsl:template name="failingruns">
    <div class="failingruns">
    <h2>Failing Runs:</h2>
    <xsl:variable name="failing" select="/plsa/run[@outcome='failure']"/>
    <table class="failingruns">
    <tbody>
    <tr>
    <th style="text-align:left">Total:</th>
    <td><xsl:value-of select="count($failing)"/></td>
    </tr>
    <tr>
    <th style="text-align:left">Claimed by some buggy aspect:</th>
    <td><xsl:value-of select="count(/plsa/aspect[@kind='bug']/runid)"/></td>
    </tr>
    </tbody>
    </table>
    <table class="cotable">
      <xsl:call-template name="cotableheader"/>  
      <tbody>
      <xsl:for-each select="plsa/aspect[@kind='bug']">
        <xsl:call-template name="cotablerow"/>
      </xsl:for-each>
      </tbody>
    </table> 
    </div>
  </xsl:template>

  <xsl:template name="aspectheader">
    <thead>
    <tr>
      <th>Aspect Number</th>
      <th>Total Runs</th>
      <th>Successes</th>
      <th>Failures</th>
      <th>Runs</th>
      <th>Features</th>
    </tr>
    </thead>
  </xsl:template>

  <xsl:template name="aspectrow">
    <xsl:variable name="runs" select="id(runid/@idref)"/>
    <xsl:variable name="succeeding" select="count($runs[@outcome='success'])"/>
    <xsl:variable name="failing" select="count($runs[@outcome='failure'])"/>
    <xsl:variable name="runslink">
      <xsl:text>./aspect</xsl:text><xsl:value-of select="@index"/><xsl:text>-runs.html</xsl:text>
    </xsl:variable>
    <xsl:variable name="featuresprefix">
      <xsl:text>./aspect</xsl:text><xsl:value-of select="@index"/><xsl:text>-features</xsl:text>
    </xsl:variable>
    <xsl:variable name="featureslink">
      <xsl:value-of select="$featuresprefix"/><xsl:text>.html</xsl:text>
    </xsl:variable>
    <xsl:variable name="featuresfile">
      <xsl:value-of select="$featuresprefix"/><xsl:text>.xml.raw</xsl:text>
    </xsl:variable>
    <tr>
      <th><xsl:value-of select="@index"/></th>
      <td><xsl:value-of select="count($runs)"/></td>
      <td><xsl:value-of select="$succeeding"/></td>
      <td><xsl:value-of select="$failing"/></td>
      <td><a href="{$runslink}">&link;</a></td>
      <td><a href="{$featureslink}"><xsl:value-of select="@ratio"/></a></td>
      <exsl:document href="{$featuresfile}" method="xml">
        <xsl:call-template name="rawfeature">
          <xsl:with-param name="num" select="@index"/>
        </xsl:call-template>
      </exsl:document>
    </tr>
  </xsl:template>

  <xsl:template name="aspectfooter">
    <xsl:param name="runs"/>
    <tfoot>
      <tr>
        <th>Totals</th>
        <td><xsl:value-of select="count($runs)"/></td>
        <td><xsl:value-of select="count($runs[@outcome='success'])"/></td>
        <td><xsl:value-of select="count($runs[@outcome='failure'])"/></td>
      </tr>
    </tfoot>
  </xsl:template>

  <xsl:template name="cotableheader">
    <thead>
    <tr>
      <th>Aspect</th>
      <xsl:for-each select="plsa/aspect[@kind='usage']">
        <th><xsl:value-of select="@index"/></th>
      </xsl:for-each>
      <th>Total</th>
    </tr>
    </thead>
  </xsl:template>

  <xsl:template name="cotablerow">
    <xsl:variable name="runs" select="id(runid/@idref)"/>
    <tr>
      <th><xsl:value-of select="@index"/></th>
      <xsl:for-each select="/plsa/aspect[@kind='usage']">
        <xsl:call-template name="cotableentry">
          <xsl:with-param name="runs" select="$runs"/>
        </xsl:call-template>
      </xsl:for-each>
      <td><xsl:value-of select="count($runs)"/></td> 
    </tr>
  </xsl:template> 

  <xsl:template name="cotableentry">
    <xsl:param name="runs"/>
    <xsl:variable name="usageid" select="@id"/>
    <td>
      <xsl:value-of select="count($runs/usageaspect[@idref = $usageid])"/>
    </td>
  </xsl:template>

</xsl:stylesheet>
