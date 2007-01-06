<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:cbi="http://www.cs.wisc.edu/cbi"
>

<xsl:output method="xml"/>

  <xsl:template match="/">
     <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE aspectfeatures SYSTEM "aspectfeatures.dtd"&gt;
     </xsl:text>
     <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="cbi:aspectfeatures">
    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="cbi:aspect">
    <xsl:copy>
      <xsl:attribute name="index">
        <xsl:value-of select="@index"/>
      </xsl:attribute>
    </xsl:copy>
  </xsl:template>  

  <xsl:template match="cbi:feature">
    <xsl:copy>
      <xsl:attribute name="index">
        <xsl:value-of select="@index"/>
      </xsl:attribute>
      <xsl:attribute name="probability">
        <xsl:value-of select="@probability"/>
      </xsl:attribute>
      <xsl:attribute name="cumulative">
        <xsl:variable name="previous" select="preceding-sibling::cbi:feature"/> 
        <xsl:variable name="sump" select="sum($previous/@probability)"/>
        <xsl:value-of select="$sump + @probability"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
