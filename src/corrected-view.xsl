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


  <!-- should we offer zoom pages? -->
  <xsl:param name="link-to-zoom" select="true()"/>


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
    <xsl:if test="$link-to-zoom">
      <th>Zoom</th>
    </xsl:if>
  </xsl:template>


  <!-- customized dynamic data cells for each predictor row -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <td><xsl:value-of select="@initial"/></td>
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates select="document('predictor-info.xml')/predictor-info/info[$index]/bug-o-meter"/>
    <td><xsl:value-of select="@effective"/></td>
    <xsl:apply-templates select="bug-o-meter"/>
    <xsl:if test="$link-to-zoom">
      <td class="link">
	<a href="zoom-{/scores/@projection}-{@index}.xml">&link;</a>
      </td>
    </xsl:if>
  </xsl:template>


</xsl:stylesheet>
