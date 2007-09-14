<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="xhtml"
  >

  <xsl:output
    method="xml"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    />

  <xsl:template match="xhtml:body/xhtml:pre/xhtml:tt">
    <xsl:apply-templates select="xhtml:a" mode="overlay"/>
  </xsl:template>

  <xsl:template match="xhtml:a" mode="overlay">
    <a id="{@name}">
      <xsl:variable name="here" select="."/>
      <!-- successor nodes which are not line anchors -->
      <!-- and whose immediately-preceding line achor is this one -->
      <xsl:copy-of select="following-sibling::node()[not(self::xhtml:a) and preceding-sibling::xhtml:a[1] = $here]"/>
    </a>
  </xsl:template>

  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy> 
  </xsl:template>

</xsl:stylesheet>
