<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="xhtml"
  >

  <xsl:template match="xhtml:body/xhtml:pre/xhtml:tt">
    <xsl:apply-templates select="xhtml:a" mode="overlay"/>
  </xsl:template>

  <xsl:template match="xhtml:a" mode="overlay">
    <xsl:copy-of select="."/>
    <span id="{@name}">
      <xsl:variable name="here" select="."/>
      <!-- successor nodes which are not line anchors -->
      <!-- and whose immediately-preceding line achor is this one -->
      <xsl:copy-of select="following-sibling::node()[not(self::xhtml:a) and preceding-sibling::xhtml:a[1] = $here]"/>
    </span>
  </xsl:template>

  <xsl:template match="*">
    <xsl:copy select=".">
      <xsl:apply-templates/>
    </xsl:copy> 
  </xsl:template>

</xsl:stylesheet>
