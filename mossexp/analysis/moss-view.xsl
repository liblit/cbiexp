<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:import href="view.xsl"/>

  <!-- add column headers for our own additional information -->
  <xsl:template mode="headings" match="view">
    <xsl:apply-imports/>
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
  <xsl:template mode="cells" match="predictor">
    <xsl:apply-imports/>
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates mode="histogram" select="document('histograms.xml')/histograms/predictor[$index]"/>
  </xsl:template>

  <xsl:template mode="histogram" match="bug">
    <td class="count">
      <xsl:value-of select="@count"/>
    </td>
  </xsl:template>

</xsl:stylesheet>
