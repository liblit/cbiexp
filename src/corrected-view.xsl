<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="view.xsl"/>


  <!-- extra column headers for our own additional information -->
  <xsl:template mode="dynamic-headings" match="view">
    <th>Initial Score</th>
    <th>Initial Bug-O-Meter&#8482;</th>
    <th>Effective Score</th>
    <th>Effective Bug-O-Meter&#8482;</th>
  </xsl:template>


  <!-- extra columns with popularity information -->
  <xsl:template mode="dynamic-cells" match="predictor">
    <td><xsl:value-of select="@initial"/></td>
    <xsl:apply-imports/>
    <td><xsl:value-of select="@effective"/></td>
    <xsl:apply-templates select="bug-o-meter"/>
  </xsl:template>


</xsl:stylesheet>
