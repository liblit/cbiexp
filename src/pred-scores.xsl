<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>


  <!-- extra column headings -->
  <xsl:template match="scores" mode="dynamic-headings">
    <xsl:apply-imports/>
    <th>Thermometer</th>
    <th>Stat 1</th>
    <th>Stat 2</th>
  </xsl:template>


  <!-- extra predictor row cells -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <xsl:apply-imports/>
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates select="document('predictor-info.xml')/predictor-info/info[$index]/bug-o-meter"/>
    <td><xsl:value-of select="@stat1"/></td>
    <td><xsl:value-of select="@stat2"/></td>
  </xsl:template>


</xsl:stylesheet>
