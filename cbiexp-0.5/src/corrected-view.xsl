<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>


  <!-- customized page CSS stylesheet -->
  <xsl:template match="/" mode="css">
    view.css
  </xsl:template>


  <!-- customized dynamic data column headers for the big table -->
  <xsl:template match="scores" mode="dynamic-headings">
    <th>Initial Score</th>
    <th>Initial Thermometer</th>
    <th>Effective Score</th>
    <th>Effective Thermometer</th>
    <th>Zoom</th>
  </xsl:template>


  <!-- customized dynamic data cells for each predictor row -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <td><xsl:value-of select="@initial"/></td>
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates select="document('predictor-info.xml', /)/predictor-info/info[$index]/bug-o-meter"/>
    <td><xsl:value-of select="@effective"/></td>
    <xsl:apply-templates select="bug-o-meter"/>
    <td class="link">
      <a href="zoom-{/scores/@projection}-{@index}.xml">
	&link;
      </a>
    </td>
  </xsl:template>


</xsl:stylesheet>
