<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >

  <xsl:import href="operands.xsl"/>

  <!-- floating point format string for predictor scores -->
  <xsl:param name="score-format" select="'0.000'"/>

  <xsl:output
    method="text"
    encoding="ascii"/>


  <!-- row for a single retained predictor -->
  <xsl:template match="predictor">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:variable name="info" select="document('predictor-info.xml')/predictor-info/info[$index]"/>

    <!-- initial score and bug-o-meter -->
    <xsl:value-of select="format-number(@initial, $score-format)"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:apply-templates select="$info/bug-o-meter"/>

    <!-- effective score and bug-o-meter -->
    <xsl:text> &amp; </xsl:text>
    <xsl:value-of select="format-number(@effective, $score-format)"/>
    <xsl:text> &amp; </xsl:text>
    <xsl:apply-templates select="bug-o-meter"/>

    <!-- source code information -->
    <xsl:text> &amp; </xsl:text>
    <xsl:variable name="operands">
      <xsl:apply-templates mode="operands" select="$info"/>
    </xsl:variable>
    <xsl:text>\verb|</xsl:text>
    <xsl:value-of select="normalize-space($operands)"/>
    <xsl:text>|</xsl:text>

    <!-- bug co-occurrence counts -->
    <xsl:apply-templates mode="histogram" select="document('histograms.xml')/histograms/predictor[$index]"/>

    <xsl:text> \\
</xsl:text>
  </xsl:template>


  <!-- a single Bug-O-Meter -->
  <xsl:template match="bug-o-meter">
    <xsl:variable name="width" select="@log10-true"/>
    <xsl:text>\bugometer{</xsl:text>
    <xsl:value-of select="$width"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="@context"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="@lower-bound"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="@fail - (@context + @lower-bound)"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="1 - @fail"/>
    <xsl:text>}</xsl:text>
  </xsl:template>


  <!-- a single bug co-occurrence count -->
  <xsl:template mode="histogram" match="bug">
    <xsl:choose>
      <xsl:when test="position() = 8"/>
      <xsl:when test="position() = 10"/>
      <xsl:otherwise>
	<xsl:text> &amp; </xsl:text>
	<xsl:value-of select="@count"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


</xsl:stylesheet>
