<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="corrected-view.xsl"/>
  <xsl:import href="histograms.xsl"/>

  <xsl:template match="scores" mode="scores-table-headings">
    <xsl:apply-imports/>
    <xsl:apply-templates mode="histogram-headings" select="."/>
  </xsl:template>

  <xsl:template match="predictor" mode="predictor-row-cells">
    <xsl:apply-imports/>
    <xsl:apply-templates mode="histogram-row-cells" select="."/>
  </xsl:template>

</xsl:stylesheet>
