<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:import href="view.xsl"/>

  <!-- add column headers for our own additional information -->
  <xsl:template match="projected-view">
    <table class="predictors">
      <tr>
	<th>initial<br/>popularity</th>
	<th>effective<br/>popularity</th>
	<xsl:call-template name="view-headings"/>
      </tr>
      <xsl:apply-templates/>
    </table>
  </xsl:template>

  <!-- add extra columns with popularity information -->
  <xsl:template match="predictor">
    <tr class="pred">
      <xsl:apply-templates select="popularity"/>
      <xsl:call-template name="predictor-cells"/>
    </tr>
  </xsl:template>

  <!-- popularity stats for one predicate -->
  <xsl:template match="popularity">
    <td>
      <xsl:value-of select="@initial"/>
    </td>
    <td>
      <xsl:value-of select="@effective"/>
    </td>
  </xsl:template>

</xsl:stylesheet>
