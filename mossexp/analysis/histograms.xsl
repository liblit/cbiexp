<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">


  <!-- add column headers for our own additional information -->
  <xsl:template match="scores" mode="histogram-headings">
    <th>#1</th>
    <th>#2</th>
    <th>#3</th>
    <th>#4</th>
    <th>#5</th>
    <th>#6</th>
    <th>#7</th>
    <th>#8</th>
    <th>#9</th>
    <th>#10</th>
  </xsl:template>


  <!-- add table cells with our own additional information -->
  <xsl:template match="predictor" mode="histogram-row-cells">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates mode="histogram" select="document('histograms.xml')/histograms/predictor[$index]"/>
  </xsl:template>

  <xsl:template match="bug" mode="histogram">
    <td class="count">
      <xsl:number select="@count"/>
    </td>
  </xsl:template>


</xsl:stylesheet>
