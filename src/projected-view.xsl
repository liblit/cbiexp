<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:import href="view.xsl"/>


  <!-- add our own related links -->
  <xsl:template name="go-to-links" match="scores">
    <xsl:apply-imports/>
    <xsl:if test="@scheme = 'all' and (@sort = 'hl' or @sort = 'hs')">
      [<a href="top-rho_{@sort}_{@projection}.xml">correlation matrix for top predictors</a>]
    </xsl:if>
  </xsl:template>

  <!-- extra column headers for our own additional information -->
  <xsl:template mode="dynamic-headings" match="scores">
    <th>Initial Popularity</th>
    <th>Effective Popularity</th>
    <xsl:apply-imports/>
  </xsl:template>

  <!-- extra columns with popularity information -->
  <xsl:template mode="dynamic-cells" match="predictor">
    <td><xsl:value-of select="@initial"/></td>
    <td><xsl:value-of select="@effective"/></td>
    <xsl:apply-imports/>
  </xsl:template>

</xsl:stylesheet>
