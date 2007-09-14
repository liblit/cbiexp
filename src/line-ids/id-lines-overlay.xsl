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
      <!-- successor nodes whose immediately-preceding a is this one -->
      <xsl:apply-templates select="following-sibling::node()[preceding-sibling::xhtml:a[1] = $here]" mode="underlay"/>
    </span>
  </xsl:template>

  <xsl:template match="xhtml:a" mode="underlay"/>

  <xsl:template match="*" mode="underlay">
    <xsl:copy-of select="."/>
  </xsl:template>

  <xsl:template match="*">
    <xsl:copy select=".">
      <xsl:apply-templates/>
    </xsl:copy> 
  </xsl:template>

</xsl:stylesheet>
