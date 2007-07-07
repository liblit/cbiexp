<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >

  <xsl:import href="bug-o-meter.xsl"/>
  <xsl:import href="operands.xsl"/>
  <xsl:import href="corrected-view.xsl"/>

  <xsl:output
    method="text"
    encoding="ascii"/>


  <!-- bug co-occurrence counts -->
  <xsl:template name="extra-columns">
    <xsl:param name="index"/>
    <xsl:apply-templates mode="histogram" select="document('histograms.xml')/histograms/predictor[$index]"/>
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
