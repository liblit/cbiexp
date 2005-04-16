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
    <xsl:value-of select="operand[1]/@source"/> &gt;= 0
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
    <xsl:value-of select="operand[1]/@source"/> &lt;= 0
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 0]">
    <xsl:apply-templates mode="operands-scalar-pairs" select=".">
      <xsl:with-param name="order" select="'&lt;'"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 1]">
    <xsl:apply-templates mode="operands-scalar-pairs" select=".">
      <xsl:with-param name="order" select="'&gt;='"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 2]">
    <xsl:apply-templates mode="operands-scalar-pairs" select=".">
      <xsl:with-param name="order" select="'=='"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 3]">
    <xsl:apply-templates mode="operands-scalar-pairs" select=".">
      <xsl:with-param name="order" select="'!='"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 4]">
    <xsl:apply-templates mode="operands-scalar-pairs" select=".">
      <xsl:with-param name="order" select="'&gt;'"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="operands" match="*[@scheme = 'scalar-pairs' and @predicate = 5]">
    <xsl:apply-templates mode="operands-scalar-pairs" select=".">
      <xsl:with-param name="order" select="'&lt;='"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="operands-scalar-pairs" match="*">
    <xsl:param name="order"/>
    <xsl:if test="operand[1]/@source = operand[2]/@source">
      <xsl:text>new value of </xsl:text>
    </xsl:if>
    <xsl:value-of select="operand[1]/@source"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$order"/>
    <xsl:text> </xsl:text>
    <xsl:if test="operand[1]/@source = operand[2]/@source">
      <xsl:text>old value of </xsl:text>
    </xsl:if>
    <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="*">
    <xsl:message terminate="yes">
      Cannot format source for scheme <xsl:value-of select="@scheme"/>, predicate <xsl:value-of select="@predicate"/>.
    </xsl:message>
  </xsl:template>

</xsl:stylesheet>
