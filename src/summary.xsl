<?xml version="1.0"?>
  <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output
    method="html"
    indent="yes"
    doctype-system="http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-strict.dtd"
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"/>

  <xsl:variable name="projectURL" select="'http://www.cs.berkeley.edu/~liblit/sampler/'"/>
  <xsl:variable name="prefix" select="/experiment/prefix"/>

  <xsl:template match="/experiment">
    <html>
      <head>
	<title>Cooperative Bug Isolation Report</title>
	<link rel="stylesheet" href="summary.css" type="text/css"/>
      </head>

      <body>
	<!-- generic page header --> 
	<h1><a href="{$projectURL}" class="logo"><img
	src="{$projectURL}logo.png" alt="" style="border-style:
	none"/></a>Cooperative Bug Isolation Report</h1>

	<!-- miscellaneous information about the experiment -->
	<p>Experiment name: <xsl:value-of select="name"/></p>
	<p>Generated on <xsl:value-of select="date"/></p>

	<!-- outcome summary -->
	<p># runs: <xsl:value-of select="sum(runs/*)"/>
	[successful: <xsl:value-of select="runs/success"/>
	failing: <xsl:value-of select="runs/fail"/>
	discarded: <xsl:value-of select="runs/ignore"/>]
	</p>

	<!-- predicate counts before filtering -->
	<p># predicates instrumented:
	  <xsl:value-of	select="sum(scheme/total)"/>
	  [<xsl:for-each select="scheme">
	    <xsl:value-of select="name"/>:
	    <xsl:value-of select="total"/>
	    <xsl:if test="position() != last()">
	      <xsl:text> </xsl:text>
	    </xsl:if>
	  </xsl:for-each>]
	</p>

	<!-- predicate counts after filtering -->
	<p># predicates retained:
	  <xsl:value-of	select="sum(scheme/retain)"/>
	  [<xsl:for-each select="scheme">
	    <xsl:value-of select="name"/>:
	    <xsl:value-of select="retain"/>
	    <xsl:if test="position() != last()">
	      <xsl:text> </xsl:text>
	    </xsl:if>
	  </xsl:for-each>]
	</p>

	<!-- miscellaneous information about the analysis -->
	<p>Confidence: <xsl:value-of select="confidence"/>%</p>

	<!-- how many schemes retained at least one predicate? -->
	<xsl:variable name="interesting" select="count(scheme[retain &gt; 0])"/>

	<!-- table of links to per-scheme pages -->
	<!-- omit entire table if nothing at all was retained -->
	<xsl:if test="$interesting &gt; 0">
	  <table>
	    <tr>
	      <th>Sorted by:</th>
	      <th>lower bound of confidence interval</th>
	      <th>increase score</th>
	      <th>fail score</th>
	      <th>true in # F runs</th>
	    </tr>

	    <!-- one row per scheme with retained predicates -->
	    <xsl:for-each select="scheme[retain &gt; 0]">
	      <xsl:call-template name="scheme-links-row">
		<xsl:with-param name="name" select="name"/>
		<xsl:with-param name="code" select="code"/>
	      </xsl:call-template>
	    </xsl:for-each>

	    <!-- final row with links for all schemes -->
	    <!-- if there was more than one scheme above -->
	    <xsl:if test="$interesting &gt; 1">
	      <xsl:call-template name="scheme-links-row">
		<xsl:with-param name="name" select="'all'"/>
		<xsl:with-param name="code" select="'preds'"/>
	      </xsl:call-template>
	    </xsl:if>
	  </table>
	</xsl:if>
      </body>
    </html>
  </xsl:template>

  <!-- named template for generating one row of links -->
  <xsl:template name="scheme-links-row">
    <xsl:param name="name"/> <!-- human-readable scheme name -->
    <xsl:param name="code"/> <!-- sort scheme code for file names -->
    <tr>
      <td class="scheme"><xsl:value-of select="$name"/></td>
      <td><a href="{$code}_lb.{$prefix}.html">X</a></td>
      <td><a href="{$code}_is.{$prefix}.html">X</a></td>
      <td><a href="{$code}_fs.{$prefix}.html">X</a></td>
      <td><a href="{$code}_nf.{$prefix}.html">X</a></td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
