<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="operands.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


  <!-- main master template for generated page -->
  <xsl:template match="/">
    <html>
      <head>
	<title>Predictor Scores</title>
	<link rel="stylesheet" href="view.css" type="text/css"/>
      </head>

      <body>
	<xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>


  <!-- table of predictors -->
  <xsl:template match="scores">
    <table class="predictors">
      <thead>
	<tr>
	  <xsl:apply-templates mode="dynamic-headings" select="."/>
	  <xsl:apply-templates mode="static-headings" select="."/>
	</tr>
      </thead>
      <tbody>
	<xsl:apply-templates/>
      </tbody>
    </table>
  </xsl:template>

  <!-- default headings for the big predictor table -->
  <xsl:template mode="dynamic-headings" match="scores">
    <th>Score</th>
  </xsl:template>

  <xsl:template mode="static-headings" match="scores">
    <th>Predicate</th>
    <th>Function</th>
    <th>File:Line</th>
  </xsl:template>


  <!-- one predictor row -->
  <xsl:template match="predictor">
    <tr>
      <xsl:variable name="index" select="number(@index)"/>
      <xsl:attribute name="title">rank: <xsl:number/>, pred: <xsl:value-of select="@index"/></xsl:attribute>
      <xsl:copy-of select="document('predictor-info.xml')/predictor-info/info[$index]/@class"/>
      <xsl:apply-templates mode="dynamic-cells" select="."/>
      <xsl:apply-templates mode="static-cells" select="."/>
    </tr>
  </xsl:template>


  <!-- default row cells for the big predictor table -->

  <!-- simple score value in a single table cell -->
  <xsl:template mode="dynamic-cells" match="predictor">
    <td><xsl:value-of select="@score"/></td>
  </xsl:template>

  <!-- several cells depending only on static predicate information -->
  <xsl:template mode="static-cells" match="predictor">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:variable name="info" select="document('predictor-info.xml')/predictor-info/info[$index]"/>
    <xsl:if test="not($info)">
      <xsl:message terminate="yes">
	<xsl:text>No static information for predictor index </xsl:text>
	<xsl:number value="$index"/>
	<xsl:text>.</xsl:text>
      </xsl:message>
    </xsl:if>
    <xsl:apply-templates mode="static-cells" select="$info"/>
  </xsl:template>

  <!-- operands, function name, and link to source -->
  <xsl:template mode="static-cells" match="info">
    <td>
      <xsl:apply-templates mode="operands" select="."/>
    </td>
    <td><xsl:value-of select="@function"/>()</td>
    <td>
      <xsl:apply-templates select="." mode="source-link"/>
    </td>
  </xsl:template>

  <!-- link to a single line of source code -->
  <xsl:template match="info" mode="source-link">
    <xsl:variable name="source-dir" select="document('summary.xml')/experiment/@source-dir"/>
    <xsl:variable name="prefix">
      <xsl:choose>
	<xsl:when test="starts-with(@file, '/')"/>
	<xsl:when test="not($source-dir)"/>
	<xsl:otherwise>
	  <xsl:value-of select="concat($source-dir, '/')"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <a href="{$prefix}{@file}.html#line{@line}">
      <xsl:value-of select="@file"/>:<xsl:value-of select="@line"/>
    </a>
  </xsl:template>


</xsl:stylesheet>
