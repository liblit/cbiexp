<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >

  <xsl:import href="bug-o-meter.xsl"/>
  <xsl:import href="operands.xsl"/>

  <!-- maximum number of predictors to display -->
  <xsl:param name="predictor-limit" select="14"/>

  <!-- floating point format -->
  <xsl:param name="score-format" select="'0.000'"/>

  <xsl:output
    method="text"
    encoding="ascii"/>


  <!-- sequence of predictor rows + trailing ellision -->
  <xsl:template match="view">
    <xsl:apply-templates/>
    <xsl:if test="count(*) > $predictor-limit">
    <xsl:text>\multicolumn{8}{c}{\dotfill{} </xsl:text>
    <xsl:value-of select="count(*) - $predictor-limit"/>
    <xsl:text> additional predictors follow \dotfill{}} \\
</xsl:text>
    </xsl:if>
  </xsl:template>


  <!-- row for a single retained predictor -->
  <xsl:template match="predictor">
    <xsl:if test="position() &lt;= $predictor-limit">
      <xsl:variable name="index" select="number(@index)"/>
      <xsl:variable name="info" select="document('predictor-info.xml')/predictor-info/info[$index]"/>

      <!-- Bug-O-Meter and constituent quantities -->
      <xsl:choose>
	<xsl:when test="bug-o-meter">
	  <!-- effective -->
	  <xsl:apply-templates select="bug-o-meter"/>
	</xsl:when>
	<xsl:otherwise>
	  <!-- initial -->
	  <xsl:apply-templates select="$info/bug-o-meter"/>
	</xsl:otherwise>
      </xsl:choose>

      <!-- source code information -->
      <xsl:text> &amp; </xsl:text>
      <xsl:variable name="operands">
	<xsl:apply-templates mode="operands" select="$info"/>
      </xsl:variable>
      <xsl:text>\verb|</xsl:text>
      <xsl:value-of select="normalize-space($operands)"/>
      <xsl:text>|</xsl:text>

      <xsl:text> \\
</xsl:text>
    </xsl:if>
  </xsl:template>


  <!-- a single Bug-O-Meter and constituent quantities -->
  <xsl:template match="bug-o-meter">

    <!-- the actual Bug-O-Meter -->
    <xsl:apply-imports/>

    <!-- constituent quantities -->
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="format-number(@context, $score-format)"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="format-number(@increase, $score-format)"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="format-number(2 * (@increase - @lower-bound), $score-format)"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="@true-success"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="@true-failure"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="@true-success + @true-failure"/>
    
  </xsl:template>


</xsl:stylesheet>
