<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

<xsl:output method="xml"/>

  <xsl:template match="/">
     <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE plsa SYSTEM "summary.dtd"&gt;
     </xsl:text>
     <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="plsa">
    <xsl:copy>
      <xsl:apply-templates select="aspect"/>
      <xsl:apply-templates select="run"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="aspect">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:value-of select="generate-id(.)"/>
      </xsl:attribute>
      <xsl:attribute name="index">
        <xsl:value-of select="position()"/>
      </xsl:attribute>
      <xsl:apply-templates select="@kind | @ratio | @maxrunlength | @minrunlength | @meanrunlength | @runlengthstd | node()"/> 
    </xsl:copy>
  </xsl:template>  

  <xsl:template match="run">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:value-of select="generate-id(.)"/>
      </xsl:attribute>
      <xsl:attribute name="index">
        <xsl:value-of select="position()"/>
      </xsl:attribute>
      <xsl:apply-templates select="@outcome | @totalcount"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="runid">
    <xsl:variable name="runindex" select="@index"/>
    <xsl:copy>
      <xsl:attribute name="idref">
        <xsl:value-of select="generate-id(//run[number($runindex)])"/>
      </xsl:attribute>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="featureclaimed">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()" />
    </xsl:copy>
  </xsl:template>

  <xsl:template match="usageaspect|buggyaspect">
    <xsl:variable name="aspectindex" select="@index"/>
    <xsl:copy>
      <xsl:attribute name="idref">
        <xsl:value-of select="generate-id(//aspect[number($aspectindex)])"/>
      </xsl:attribute>
      <xsl:apply-templates select="@probability | node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="@*">
    <xsl:copy/>
  </xsl:template>

</xsl:stylesheet>
