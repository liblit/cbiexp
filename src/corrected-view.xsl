<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="view.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


  <!-- should we offer zoom pages? -->
  <xsl:param name="link-to-zoom" select="true()"/>


  <!-- extra column headers for our own additional information -->
  <xsl:template mode="dynamic-headings" match="view">
    <th>Initial Score</th>
    <th>Initial Thermometer</th>
    <th>Effective Score</th>
    <th>Effective Thermometer</th>
    <xsl:if test="$link-to-zoom">
      <th>Zoom</th>
    </xsl:if>
  </xsl:template>


  <!-- extra columns with popularity information -->
  <xsl:template mode="dynamic-cells" match="predictor">
    <td><xsl:value-of select="@initial"/></td>
    <xsl:apply-imports/>
    <td><xsl:value-of select="@effective"/></td>
    <xsl:apply-templates select="bug-o-meter"/>
    <xsl:if test="$link-to-zoom">
      <td class="link">
	<xsl:call-template name="zoom-link">
	  <xsl:with-param name="projection" select="@projection"/>
	  <xsl:with-param name="index" select="@index"/>
	</xsl:call-template>
      </td>
    </xsl:if>
  </xsl:template>


  <!-- hyperlink for a single predicate's zoom page -->
  <xsl:template name="zoom-link">
    <xsl:param name="projection"/>
    <xsl:param name="index"/>
    <a href="zoom-{/view/@projection}-{@index}.xml">&link;</a>
  </xsl:template>


</xsl:stylesheet>
