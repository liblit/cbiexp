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

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


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
      <th class="label"><xsl:value-of select="$title"/>:&#160;</th>
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
      <thead>
	<tr class="headers">
	  <th>Scheme</th>
	  <th colspan="4">Predicates<br/>Retained</th>
	  <xsl:for-each select="document('sorts.xml')/sorts/sort">
	    <th><xsl:apply-templates/></th>
	  </xsl:for-each>
	</tr>
      </thead>

      <!-- summary row -->
      <tfoot>
	<xsl:call-template name="scheme-links-row">
	  <xsl:with-param name="name" select="'all'"/>
	  <xsl:with-param name="retain" select="sum(*/@retain)"/>
	  <xsl:with-param name="total" select="sum(*/@total)"/>
	</xsl:call-template>
      </tfoot>

      <!-- one row per scheme -->
      <tbody>
	<xsl:apply-templates/>
      </tbody>

    </table>
  </xsl:template>


  <!-- table row for a single instrumentation scheme -->
  <xsl:template match="scheme">
    <xsl:call-template name="scheme-links-row">
      <xsl:with-param name="name" select="@name"/>
      <xsl:with-param name="retain" select="@retain"/>
      <xsl:with-param name="total" select="@total"/>
    </xsl:call-template>
  </xsl:template>


  <!-- named template for generating one row of links -->
  <xsl:template name="scheme-links-row">
    <xsl:param name="name"/> <!-- scheme name -->
    <xsl:param name="retain"/> <!-- number of predicates retained -->
    <xsl:param name="total"/> <!-- number of predicates before filtering -->
    <xsl:if test="$total > 0">
      <tr>
	<td class="label"><xsl:value-of select="$name"/></td>
	<td class="retain count"><xsl:number value="$retain" grouping-size="3" grouping-separator=","/></td>
	<td class="solidus">&#160;of&#160;</td>
	<td class="total count"><xsl:number value="$total" grouping-size="3" grouping-separator=","/></td>
	<td class="percent">&#160;(<xsl:number value="round($retain div $total * 100)"/>%)</td>
	<xsl:for-each select="document('sorts.xml')/sorts/sort">
	  <xsl:choose>
	    <xsl:when test="$retain = 0">
	      <td class="link"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <td class="link"><a href="{$name}_{@code}_none.xml">&link;</a></td>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </tr>
    </xsl:if>
  </xsl:template>


</xsl:stylesheet>
