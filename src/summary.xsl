<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9760;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <xsl:import href="logo.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


  <xsl:variable name="prefix" select="/experiment/analysis/@prefix"/>


  <!-- main master template for generated page -->
  <xsl:template match="/experiment">
    <html>
      <head>
	<title>Cooperative Bug Isolation Report</title>
	<link rel="stylesheet" href="summary.css" type="text/css"/>
      </head>

      <body>
	<!-- generic page header -->
	<h1>
	  <xsl:copy-of select="$logo-icon"/>
	  Cooperative Bug Isolation Report
	</h1>

	<!-- miscellaneous information about the experiment -->
	<p>Experiment name: <xsl:value-of select="@title"/></p>
	<p>Generated on <xsl:value-of select="@date"/></p>

	<xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>


  <!-- outcome summary -->
  <xsl:template match="runs">
    <xsl:variable name="total" select="sum(@*)"/>
    <p class="outcomes"><xsl:number value="$total" grouping-size="3" grouping-separator=","/> runs:</p>
    <table class="outcomes">
      <xsl:call-template name="run-label">
	<xsl:with-param name="title" select="'Successes'"/>
	<xsl:with-param name="count" select="@success"/>
	<xsl:with-param name="total" select="$total"/>
      </xsl:call-template>
      <xsl:call-template name="run-label">
	<xsl:with-param name="title" select="'Failures'"/>
	<xsl:with-param name="count" select="@failure"/>
	<xsl:with-param name="total" select="$total"/>
      </xsl:call-template>
      <xsl:call-template name="run-label">
	<xsl:with-param name="title" select="'Discards'"/>
	<xsl:with-param name="count" select="@ignore"/>
	<xsl:with-param name="total" select="$total"/>
      </xsl:call-template>
    </table>
  </xsl:template>

  <!-- an individual outcome type -->
  <xsl:template name="run-label">
    <xsl:param name="title"/>
    <xsl:param name="count"/>
    <xsl:param name="total"/>
    <tr>
      <td class="label"><strong><xsl:value-of select="$title"/>:</strong>&#160;</td>
      <td class="count"><xsl:number value="$count" grouping-size="3" grouping-separator=","/></td>
      <td class="percent">&#160;(<xsl:number value="round($count div $total * 100)"/>%)</td>
    </tr>
  </xsl:template>


  <!-- miscellaneous information about the analysis -->
  <xsl:template match="analysis">
    <p>Confidence: <xsl:value-of select="@confidence"/>%</p>
  </xsl:template>


  <!-- table of links to per-scheme pages -->
  <xsl:template match="schemes">
    <table class="scheme-links">
      <tr class="headers">
	<th>Scheme</th>
	<th colspan="4">Predicates Retained</th>
	<th>Lower Bound of Confidence Interval</th>
	<th>Increase Score</th>
	<th>Fail Score</th>
	<th>True in # Failed Runs</th>
      </tr>

      <!-- one row per scheme -->
      <xsl:apply-templates/>

      <!-- summary row -->
      <xsl:call-template name="scheme-links-row">
	<xsl:with-param name="name" select="'all'"/>
	<xsl:with-param name="code" select="'preds'"/>
	<xsl:with-param name="retain" select="sum(*/@retain)"/>
	<xsl:with-param name="total" select="sum(*/@total)"/>
      </xsl:call-template>

    </table>
  </xsl:template>


  <!-- table row for a single instrumentation scheme -->
  <xsl:template match="scheme">
    <xsl:call-template name="scheme-links-row">
      <xsl:with-param name="name" select="@name"/>
      <xsl:with-param name="code" select="@code"/>
      <xsl:with-param name="retain" select="@retain"/>
      <xsl:with-param name="total" select="@total"/>
    </xsl:call-template>
  </xsl:template>


  <!-- named template for generating one row of links -->
  <xsl:template name="scheme-links-row">
    <xsl:param name="name"/> <!-- human-readable scheme name -->
    <xsl:param name="code"/> <!-- sort scheme code for file names -->
    <xsl:param name="retain"/> <!-- number of predicates retained -->
    <xsl:param name="total"/> <!-- number of predicates before filtering -->
    <xsl:if test="$total > 0">
      <tr class="{$name}">
	<td class="label"><xsl:value-of select="$name"/></td>
	<td class="retain count"><xsl:number value="$retain" grouping-size="3" grouping-separator=","/></td>
	<td class="solidus">&#160;of&#160;</td>
	<td class="total count"><xsl:number value="$total" grouping-size="3" grouping-separator=","/></td>
	<td class="percent">&#160;(<xsl:number value="round($retain div $total * 100)"/>%)</td>
	<xsl:choose>
	  <xsl:when test="$retain = 0">
	    <td class="link"/>
	    <td class="link"/>
	    <td class="link"/>
	    <td class="link"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <td class="link"><a href="{$code}_lb.{$prefix}.xml">&link;</a></td>
	    <td class="link"><a href="{$code}_is.{$prefix}.xml">&link;</a></td>
	    <td class="link"><a href="{$code}_fs.{$prefix}.xml">&link;</a></td>
	    <td class="link"><a href="{$code}_nf.{$prefix}.xml">&link;</a></td>
	  </xsl:otherwise>
	</xsl:choose>
      </tr>
    </xsl:if>
  </xsl:template>


</xsl:stylesheet>
