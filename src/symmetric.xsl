<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="bug-o-meter.xsl"/>
  <xsl:import href="scores.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
    />


  <xsl:key name="sites" match="sites" use="concat(@unit, ':', @scheme)"/>


  <xsl:template match="/" mode="css">
    symmetric.css
  </xsl:template>


  <xsl:template match="scores" mode="dynamic-headings">
    <xsl:apply-imports/>
    <th>Thermometer</th>
  </xsl:template>


  <xsl:template match="predictor">
    <tr>
      <xsl:attribute name="title">rank: <xsl:number/></xsl:attribute>
      <xsl:apply-templates select="." mode="predictor-row-cells"/>
    </tr>
  </xsl:template>


  <xsl:template match="predictor" mode="dynamic-cells">
    <xsl:apply-imports/>
    <xsl:apply-templates select="bug-o-meter"/>
  </xsl:template>


  <xsl:template match="predictor" mode="static-cells">
    <xsl:variable name="unit-id" select="@unit"/>
    <xsl:variable name="scheme-id" select="@scheme"/>
    <xsl:variable name="site-id" select="number(@site)"/>
    <xsl:variable name="pred-id" select="@predicate"/>
    <xsl:variable name="coords" select="concat($unit-id, ':', $scheme-id)"/>

    <xsl:for-each select="document('static-site-info.xml')">
      <xsl:variable name="unit" select="key('sites', $coords)"/>
      <xsl:variable name="info" select="$unit/site[$site-id]"/>

      <xsl:if test="count($unit) != 1">
	<xsl:message terminate="yes">
	  <xsl:text>Cannot retrieve static site information for unit="</xsl:text>
	  <xsl:value-of select="$unit-id"/>
	  <xsl:text>" scheme="</xsl:text>
	  <xsl:value-of select="$scheme-id"/>
	  <xsl:text>" site="</xsl:text>
	  <xsl:value-of select="$site-id"/>
	  <xsl:text>": expected 1 node but got </xsl:text>
	  <xsl:value-of select="count($unit)"/>
	</xsl:message>
      </xsl:if>

      <td>
	<xsl:apply-templates select="$info" mode="operands">
	  <xsl:with-param name="predicate" select="$pred-id"/>
	</xsl:apply-templates>
      </td>

      <xsl:apply-templates select="$info" mode="static-cells"/>
    </xsl:for-each>
  </xsl:template>


  <xsl:template match="site[../@scheme = 'g-object-unref']" mode="operands">
    <xsl:param name="predicate"/>
    <xsl:text>old_refcount(</xsl:text>
    <xsl:value-of select="operand[1]/@source"/>
    <xsl:text>)</xsl:text>
    <xsl:text> </xsl:text>
    <xsl:value-of select="document('schemes.xml')/schemes/scheme[@id = 'g-object-unref']/predicate[number($predicate)]"/>
  </xsl:template>

  <xsl:template match="site[../@scheme = 'returns']" mode="operands">
    <xsl:param name="predicate"/>
    <xsl:value-of select="operand[1]/@source"/>
    <xsl:variable name="operator" select="document('schemes.xml')/schemes/scheme[@id = current()/../@scheme]/predicate[number($predicate)]/@text"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$operator"/>
    <xsl:text> 0</xsl:text>
  </xsl:template>

  <xsl:template match="site" mode="operands">
    <xsl:param name="predicate"/>
    <xsl:value-of select="operand[1]/@source"/>
    <xsl:variable name="operator" select="document('schemes.xml')/schemes/scheme[@id = current()/../@scheme]/predicate[number($predicate)]/@text"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$operator"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>


  <xsl:template match="site" mode="static-cells">
    <xsl:param name="test"/>
    <td><xsl:value-of select="@function"/></td>
    <!-- <xsl:variable name="source-dir" select="document('summary.xml')/experiment/@source-dir"/> -->
    <xsl:variable name="source-dir"/>
    <xsl:variable name="prefix">
      <xsl:choose>
	<xsl:when test="starts-with(@file, '/')"/>
	<xsl:when test="not($source-dir)"/>
	<xsl:otherwise>
	  <xsl:value-of select="concat($source-dir, '/')"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <td>
      <a href="{$prefix}{@file}.html#line{@line}">
	<xsl:value-of select="@file"/>:<xsl:value-of select="@line"/>
      </a>
    </td>
  </xsl:template>


</xsl:stylesheet>
