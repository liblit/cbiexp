<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
    />


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
    <td class="meter">
    <xsl:apply-templates select="bug-o-meter"/>
    </td>	
  </xsl:template>


  <xsl:template match="predictor" mode="static-cells">
    <xsl:variable name="site-id" select="number(@site)"/>
    <xsl:variable name="pred-id" select="@predicate"/>
    <xsl:variable name="sites" select="document('static-site-info.xml')//site"/>
    <xsl:variable name="info" select="$sites[number($site-id)]"/>

    <td>
        <xsl:apply-templates select="$info" mode="operands">
	    <xsl:with-param name="predicate" select="$pred-id"/>
	</xsl:apply-templates>
    </td>

      <xsl:apply-templates select="$info" mode="static-cells"/>

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
    <xsl:text>() </xsl:text>
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
    <td><xsl:value-of select="@function"/>()</td>
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
