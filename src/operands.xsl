<?xml version="1.0"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <!-- interpret a sequence of operands according to scheme and predicate number -->

  <xsl:template name="operands">
    <xsl:param name="scheme"/>
    <xsl:param name="predicate"/>

    <xsl:variable name="sources" select="operand/@source"/>
    <xsl:variable name="in-place" select="$sources[1] = $sources[2]"/>

    <xsl:if test="$in-place">
      <xsl:text>new value of </xsl:text>
    </xsl:if>
    <xsl:value-of select="$sources[1]"/>

    <xsl:variable name="operator" select="document('schemes.xml')/schemes/scheme[@id = $scheme]/predicate[$predicate]/@text"/>
    <xsl:if test="not($operator)">
      <xsl:message terminate="yes">
	<xsl:text>no descriptive operator for predicate </xsl:text>
	<xsl:value-of select="$predicate"/>
	<xsl:text> in </xsl:text>
	<xsl:value-of select="$scheme"/>
	<xsl:text> scheme</xsl:text>
      </xsl:message>
    </xsl:if>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$operator"/>

    <xsl:if test="$sources[2]">
      <xsl:text> </xsl:text>
      <xsl:if test="$in-place">
	<xsl:text>old value of </xsl:text>
      </xsl:if>
      <xsl:value-of select="$sources[2]"/>
    </xsl:if>
  </xsl:template>

  <xsl:template mode="operands" match="*">
    <xsl:call-template name="operands">
      <xsl:with-param name="scheme" select="@scheme"/>
      <xsl:with-param name="predicate" select="number(@predicate) + 1"/>
    </xsl:call-template>
  </xsl:template>

</xsl:stylesheet>
