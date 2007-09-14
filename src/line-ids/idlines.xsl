<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
>

<xsl:output method="html"/>

    <xsl:template match="xhtml:tt/xhtml:span"/>

    <xsl:template match="xhtml:a">
        <xsl:copy-of select="."/>
        <xsl:variable name="next" select="following-sibling::xhtml:span[1]/xhtml:span"/>
        <xsl:variable name="lineid" select="@name"/>
        <xsl:element name="span"> 
            <xsl:attribute name="id">
                <xsl:value-of select="$lineid"/>
            </xsl:attribute>
            <xsl:value-of select="$next"/>
        </xsl:element>
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
