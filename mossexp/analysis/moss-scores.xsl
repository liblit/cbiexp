<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>
  <xsl:import href="histograms.xsl"/>

  <xsl:template match="scores" mode="scores-table-headings">
    <xsl:apply-imports/>
    <xsl:apply-templates mode="histogram-headings" select="."/>
  </xsl:template>

  <xsl:template mode="dynamic-headings" match="scores">
    <xsl:apply-imports/>
    <th>Stat 1</th>
    <th>Stat 2</th>
  </xsl:template>

  <xsl:template mode="static-cells" match="predictor">
    <xsl:apply-imports/>
    <xsl:apply-templates mode="histogram-row-cells" select="."/>
  </xsl:template>

  <xsl:template mode="dynamic-cells" match="predictor">
    <xsl:apply-imports/>
    <td><xsl:value-of select="@stat1"/></td>
    <td><xsl:value-of select="@stat2"/></td>
  </xsl:template>

</xsl:stylesheet>
