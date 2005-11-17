<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>

  <xsl:import href="operands.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="sober-static-info.dtd"
    />


  <xsl:template match="static-site-info">
    <sober-static-info>
      <xsl:apply-templates/>
    </sober-static-info>
  </xsl:template>


  <xsl:template match="sites[@scheme = 'branches' or @scheme = 'returns']">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="sites"/>


  <xsl:template match="site">
    <xsl:variable name="site" select="."/>
    <xsl:variable name="scheme" select="../@scheme"/>

    <xsl:for-each select="document('schemes.xml')/schemes/scheme[@id = $scheme]/predicate">
      <xsl:variable name="predicate-number" select="position()"/>
      <xsl:for-each select="$site">
	<predicate file="{@file}" line="{@line}" function="{@function}">
	  <xsl:call-template name="operands">
	    <xsl:with-param name="scheme" select="$scheme"/>
	    <xsl:with-param name="predicate" select="$predicate-number"/>
	  </xsl:call-template>
	</predicate>
      </xsl:for-each>
    </xsl:for-each>

  </xsl:template>


</xsl:stylesheet>
