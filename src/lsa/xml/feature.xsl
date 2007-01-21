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
  <xsl:variable name="runs" select="document('runs.xml')/runinfos/run"/>

  <xsl:output method="html"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="cbi:aspectfeatures">
    <xsl:variable name="kind">
      <xsl:choose> 
        <xsl:when test="cbi:aspect/@kind = 'usage'">
          <xsl:text>Usage</xsl:text>
        </xsl:when>
        <xsl:when test="cbi:aspect/@kind = 'bug'">
          <xsl:text>Bug</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:message terminate="yes">Unrecognized kind of aspect.</xsl:message>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="title">Aspect <xsl:value-of select="cbi:aspect/@index"/> (<xsl:value-of select="$kind"/>)</xsl:variable>
    <html>
      <head>
        <title><xsl:value-of select="$title"/></title>
      </head>
      <body>
        <div id="frontmatter"> 
          <h1><xsl:copy-of select="$logo-icon"/><xsl:value-of select="$title"/></h1>
        </div>
        <div id="rest">
          <h2>Runs</h2>
          <xsl:apply-templates select="cbi:runstats"/>
          <h2>Features</h2> 
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

  <xsl:template name="selectrun">
    <xsl:param name="runindex"/>
    <table>
      <tbody>
        <tr>
          <th style="text-align:left">
            Index:
          </th>
          <td style="text-align:right">
            <xsl:value-of select="$runindex"/>
          </td>
        </tr>
        <xsl:variable name="run" select="$runs[position() = $runindex]"/>
        <tr>
          <th style="text-align:left">
            Length:
          </th>
          <td style="text-align:right">
            <xsl:value-of select="$run/@totalcount"/>
          </td>
        </tr>
      </tbody>
    </table>
  </xsl:template>

  <xsl:template match="cbi:runstats">
    <table>
      <tbody>
        <tr>
          <th style="text-align:left">Runs Claimed</th>
          <td style="text-align:right">
            <xsl:value-of select="@numruns"/>
          </td>
        </tr>
        <tr>
          <th style="text-align:left">Max Run Length</th>
          <td style="text-align:right">
            <xsl:value-of select="@maxrunlength"/>
          </td>
        </tr>
        <tr>
          <th style="text-align:left">Min Run Length</th>
          <td style="text-align:right">
            <xsl:value-of select="@minrunlength"/>
          </td>
        </tr>
        <tr>
          <th style="text-align:left">Mean Run Length</th>
          <td style="text-align:right">
            <xsl:value-of select="@meanrunlength"/>
          </td>
        </tr>
        <tr>
          <th style="text-align:left">Run Length Standard Deviation</th>
          <td style="text-align:right">
            <xsl:value-of select="@runlengthstd"/>
          </td>
        </tr>
        <tr>
          <th style="text-align:left">
            Best Run
            <xsl:call-template name="selectrun">
              <xsl:with-param name="runindex" select="@bestrun"/>
            </xsl:call-template>
          </th>
          <th style="text-align:left">
            Worst Run
            <xsl:call-template name="selectrun">
              <xsl:with-param name="runindex" select="@worstrun"/>
            </xsl:call-template>
          </th>
        </tr>
      </tbody>
    </table>
    <h3>Aspect Probabilities</h3> 
    <table>
      <thead>
        <xsl:call-template name="aspectprobabilityheader"/>
      </thead>
      <tbody>
        <xsl:apply-templates select="cbi:aspectprobability"/>
      </tbody>
    </table> 
  </xsl:template>

  <xsl:template match="cbi:feature">
    <xsl:variable name="index" select="@index"/>
    <xsl:variable name="mean" select="/cbi:aspectfeatures/cbi:runstats/@meanrunlength"/>
    <tr>
      <th><xsl:value-of select="@index"/></th>
      <td><xsl:value-of select="@probability"/></td>
      <td><xsl:value-of select="@cumulative"/></td>
      <td><xsl:value-of select="@bestcount"/></td> 
      <td><xsl:value-of select="@bestprediction"/></td>
      <td><xsl:value-of select="@worstcount"/></td>
      <td><xsl:value-of select="@worstprediction"/></td>
      <xsl:apply-templates select="$preds[number($index)]" mode="static-cells"/>
    </tr>
  </xsl:template>

  <xsl:template name="featureheader">
    <thead>
      <tr>
        <th>Index</th>
        <th>Probability</th>
        <th>Cumulative</th>
        <th>Best Count</th>
        <th>Best Prediction</th>
        <th>Worst Count</th>
        <th>Worst Prediction</th>
        <th>Predicate</th>
        <th>Function</th>
        <th>File:Line</th>
      </tr>
    </thead>
  </xsl:template>

  <xsl:template name="aspectprobabilityheader">
    <tr>
      <th>Aspect</th>
      <th>Mean</th>
      <th>Standard Deviation</th>
      <th>Median</th>
      <th>Maximum</th>
      <th>Minimum</th>
    </tr>
  </xsl:template>

  <xsl:template match="cbi:aspectprobability">
    <tr>
      <th><xsl:value-of select="@aspectindex"/></th>
      <td><xsl:value-of select="@mean"/></td>
      <td><xsl:value-of select="@std"/></td>
      <td><xsl:value-of select="@median"/></td>
      <td><xsl:value-of select="@max"/></td>
      <td><xsl:value-of select="@min"/></td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
