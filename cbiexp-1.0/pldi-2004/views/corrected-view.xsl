<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >

  <xsl:import href="bug-o-meter.xsl"/>
  <xsl:import href="operands.xsl"/>

  <xsl:output
    method="text"
    encoding="ascii"/>


  <!-- row for a single retained predictor -->
  <xsl:template match="predictor">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:variable name="info" select="document('predictor-info.xml')/predictor-info/info[$index]"/>

    <!-- initial and effective bug-o-meters -->
    <xsl:apply-templates select="$info/bug-o-meter"/>
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

    <xsl:call-template name="extra-columns">
      <xsl:with-param name="index" select="$index"/>
    </xsl:call-template>

    <xsl:text> \\
</xsl:text>
  </xsl:template>


  <!-- no extra columns unless overridden elsewhere -->
  <xsl:template name="extra-columns">
    <xsl:param name="index"/>
  </xsl:template>


</xsl:stylesheet>
