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
      <xsl:apply-templates select="feature"/>
      <xsl:apply-templates select="run"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="aspect">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:value-of select="generate-id(.)"/>
      </xsl:attribute>
      <xsl:attribute name="kind">
        <xsl:value-of select="@kind"/>
      </xsl:attribute> 
      <xsl:attribute name="index">
        <xsl:value-of select="position()"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>  

  <xsl:template match="feature">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:value-of select="generate-id(.)"/>
      </xsl:attribute>
      <xsl:attribute name="infoindex">
        <xsl:value-of select="@infoindex"/>
      </xsl:attribute>
      <xsl:attribute name="index">
        <xsl:value-of select="position()"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="run">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:value-of select="generate-id(.)"/>
      </xsl:attribute>
      <xsl:attribute name="outcome">
        <xsl:value-of select="@outcome"/>
      </xsl:attribute>
      <xsl:attribute name="index">
        <xsl:value-of select="position()"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="runid">
    <xsl:variable name="runindex" select="@index"/>
    <xsl:copy>
      <xsl:attribute name="idref">
        <xsl:value-of select="generate-id(//run[number($runindex)])"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="featureclaimed">
    <xsl:variable name="featureindex" select="@index"/>
    <xsl:copy>
      <xsl:attribute name="idref">
        <xsl:value-of select="generate-id(//feature[number($featureindex)])"/>
      </xsl:attribute>
      <xsl:attribute name="probability">
        <xsl:value-of select="@probability"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="usageaspect|buggyaspect">
    <xsl:variable name="aspectindex" select="@index"/>
    <xsl:copy>
      <xsl:attribute name="idref">
        <xsl:value-of select="generate-id(//aspect[number($aspectindex)])"/>
      </xsl:attribute>
      <xsl:attribute name="probability">
        <xsl:value-of select="@probability"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
