<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="xhtml"
>

<xsl:output method="html"/>

    <xsl:template match="xhtml:body/xhtml:pre/xhtml:tt/xhtml:span">
	<span id="{preceding-sibling::xhtml:a[position()=1]/@name}">
	    <xsl:copy-of select="xhtml:span/node()"/>
	</span>
    </xsl:template>

    <xsl:template match="xhtml:head">
        <xsl:copy-of select="."/>
    </xsl:template>

    <xsl:template match="*">
        <xsl:copy select=".">
        <xsl:apply-templates/>
        </xsl:copy> 
    </xsl:template>


</xsl:stylesheet>
