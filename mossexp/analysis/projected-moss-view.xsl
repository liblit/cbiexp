<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:import href="moss-view.xsl"/>

  <!-- add our own related links -->
  <xsl:template mode="go-to-links" match="view">
    <xsl:apply-imports/>
    <xsl:if test="@scheme = 'all'">
      [<a href="top-rho_{@sort}_{@projection}.xml">correlation matrix for top predictors</a>]
    </xsl:if>
  </xsl:template>

  <!-- add column headers for our own additional information -->
  <xsl:template mode="headings" match="view">
    <th>Initial Popularity</th>
    <th>Effective Popularity</th>
    <xsl:apply-imports/>
  </xsl:template>

  <!-- add table cells with our own additional information -->
  <xsl:template mode="cells" match="predictor">
    <td><xsl:value-of select="@initial"/></td>
    <td><xsl:value-of select="@effective"/></td>
    <xsl:apply-imports/>
  </xsl:template>

</xsl:stylesheet>
