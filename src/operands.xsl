<?xml version="1.0"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <!-- interpret a sequence of operands according to scheme and predicate number -->

  <xsl:template mode="operands" match="*[@scheme = 'branches' and @predicate = 0]">
    <xsl:value-of select="operand[1]/@source"/> is FALSE
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'branches' and @predicate = 1]">
    <xsl:value-of select="operand[1]/@source"/> is TRUE
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'g-object-unref' and @predicate = 0]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) == 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'g-object-unref' and @predicate = 1]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) == 1
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'g-object-unref' and @predicate = 2]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) &gt; 1
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'g-object-unref' and @predicate = 3]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) is invalid
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'returns' and @predicate = 0]">
    <xsl:value-of select="operand[1]/@source"/> &lt; 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'returns' and @predicate = 1]">
    <xsl:value-of select="operand[1]/@source"/> &#8805; 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'returns' and @predicate = 2]">
    <xsl:value-of select="operand[1]/@source"/> == 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'returns' and @predicate = 3]">
    <xsl:value-of select="operand[1]/@source"/> != 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'returns' and @predicate = 4]">
    <xsl:value-of select="operand[1]/@source"/> &gt; 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'returns' and @predicate = 5]">
    <xsl:value-of select="operand[1]/@source"/> &#8804; 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 0]">
    <xsl:value-of select="operand[1]/@source"/> &lt; <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 1]">
    <xsl:value-of select="operand[1]/@source"/> &#8805; <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 2]">
    <xsl:value-of select="operand[1]/@source"/> == <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 3]">
    <xsl:value-of select="operand[1]/@source"/> != <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 4]">
    <xsl:value-of select="operand[1]/@source"/> &gt; <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 5]">
    <xsl:value-of select="operand[1]/@source"/> &#8804; <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*">
    <xsl:message terminate="yes">
      Cannot format source for scheme <xsl:value-of select="@scheme"/>, predicate <xsl:value-of select="@predicate"/>.
    </xsl:message>
  </xsl:template>

</xsl:stylesheet>
