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
	<!-- page metainformation -->
	<title>
	  <xsl:apply-templates select="." mode="title"/>
	</title>
	<xsl:variable name="css">
	  <xsl:apply-templates select="." mode="css"/>
	</xsl:variable>
	<link rel="stylesheet" type="text/css" href="{normalize-space($css)}"/>
	<xsl:apply-templates select="." mode="head-extra"/>
      </head>
      <body>
	<xsl:apply-templates select="." mode="page-header"/>
	<xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <!-- default extra head elements -->
  <xsl:template match="/" mode="head-extra"/>

  <!-- default page title -->
  <xsl:template match="/" mode="title">
    Predictor Scores
  </xsl:template>

  <!-- default page CSS stylesheet -->
  <xsl:template match="/" mode="css">
    scores.css
  </xsl:template>

  <!-- default page header -->
  <xsl:template match="/" mode="page-header"/>


  <!-- table of predictors -->
  <xsl:template match="scores">
    <table class="predictors">
      <thead>
	<tr>
	  <xsl:apply-templates select="." mode="scores-table-headings"/>
	</tr>
      </thead>
      <tbody>
	<xsl:apply-templates/>
      </tbody>
    </table>
  </xsl:template>


  <!-- default headings for the big predictor table -->
  <xsl:template match="scores" mode="scores-table-headings">
    <xsl:apply-templates select="." mode="dynamic-headings"/>
    <xsl:apply-templates select="." mode="static-headings"/>
  </xsl:template>

  <xsl:template match="scores" mode="dynamic-headings">
    <th>Score</th>
  </xsl:template>

  <xsl:template match="scores" mode="static-headings">
    <th>Predicate</th>
    <th>Function</th>
    <th>File:Line</th>
  </xsl:template>
  
  <!-- one conjunction row -->
  <xsl:template match="conjunction">	
   
      <xsl:variable name="index" select="number(@index)"/>
      <xsl:variable name="num" 
	      select="count(document('conjunction-info.xml', /)/conjunction-info/info[@index= $index]/pred)"/>
    <tr>
      <td rowspan= "{$num}"><xsl:value-of select="@score"/></td>
      <xsl:apply-templates select="." mode="dynamic-cells"/>
      <xsl:for-each
	      select="document('conjunction-info.xml', /)/conjunction-info/info[@index= $index]/pred[1]">
	      <xsl:variable name="i" select="number(@index)"/>
	      <xsl:variable name="info" select="document('predictor-info.xml', /)/predictor-info/info[$i]"/>
	      <xsl:apply-templates select="$info" mode="static-cells"/>
      </xsl:for-each>
    </tr>
      
	      <xsl:for-each
		      select="document('conjunction-info.xml', /)/conjunction-info/info[@index= $index]/pred[position() > 1]">
	      <xsl:variable name="i" select="number(@index)"/>
	      <xsl:variable name="info" select="document('predictor-info.xml', /)/predictor-info/info[$i]"/>
	      <tr> <xsl:apply-templates select="$info" mode="static-cells"/> </tr>
      </xsl:for-each>
    	      
  </xsl:template>

  <!-- one predictor row -->
  <xsl:template match="predictor">
    <tr>
      <xsl:variable name="index" select="number(@index)"/>
      <xsl:attribute name="title">rank: <xsl:number/>, pred: <xsl:value-of select="@index"/></xsl:attribute>
      <xsl:copy-of select="document('predictor-info.xml', /)/predictor-info/info[$index]/@class"/>
      <xsl:apply-templates select="." mode="predictor-row-cells"/>
    </tr>
  </xsl:template>


  <!-- default cells for each predictor row in the big predictor table -->
  <xsl:template match="predictor" mode="predictor-row-cells">
    <xsl:apply-templates select="." mode="dynamic-cells"/>
    <xsl:apply-templates select="." mode="static-cells"/>
  </xsl:template>


  <!-- simple score value in a single table cell -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <td><xsl:value-of select="@score"/></td>
  </xsl:template>


  <!-- several cells depending only on static predicate information -->
  <xsl:template match="predictor" mode="static-cells">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:variable name="info" select="document('predictor-info.xml', /)/predictor-info/info[$index]"/>
    <xsl:if test="not($info)">
      <xsl:message terminate="yes">
	<xsl:text>No static information for predictor index </xsl:text>
	<xsl:number value="$index"/>
	<xsl:text>.</xsl:text>
      </xsl:message>
    </xsl:if>
    <xsl:apply-templates select="$info" mode="static-cells"/>
  </xsl:template>

  <!-- operands, function name, and link to source -->
  <xsl:template match="info" mode="static-cells">
    <td><xsl:apply-templates select="." mode="operands"/></td>
    <td><xsl:value-of select="@function"/>()</td>
    <td><xsl:apply-templates select="." mode="source-link"/></td>
  </xsl:template>

  <!-- link to a single line of source code -->
  <xsl:template match="info" mode="source-link">
    <xsl:variable name="source-dir" select="document('summary.xml', /)/experiment/@source-dir"/>
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


