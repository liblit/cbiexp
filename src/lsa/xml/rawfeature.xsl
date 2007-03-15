<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <xsl:output method="xml"/>

  <xsl:variable name="features" select="document('features.xml')/featureinfos/feature"/>
  <xsl:variable name="runs" select="document('runs.xml')/runinfos/run"/>

  <xsl:template name="rawfeature">
    <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE aspectfeatures SYSTEM "rawaspectfeatures.dtd"&gt;
    </xsl:text>
    <xsl:param name="num"/>
    <xsl:apply-templates select="/plsa/aspect[@index = $num]" mode="rawfeature"/>
  </xsl:template>

  <xsl:template match="aspect" mode="rawfeature">
    <xsl:element name="aspectfeatures" namespace="http://www.cs.wisc.edu/cbi">
      <xsl:element name="aspect" namespace="http://www.cs.wisc.edu/cbi">
        <xsl:attribute name="index">
          <xsl:value-of select="@index"/>
        </xsl:attribute>
        <xsl:attribute name="kind">
          <xsl:value-of select="@kind"/>
        </xsl:attribute>
      </xsl:element>
      <xsl:apply-templates select="featureclaimed" mode="rawfeature"/>
      <xsl:apply-templates select="runid" mode="rawfeature"/>
      <xsl:element name="runstats" namespace="http://www.cs.wisc.edu/cbi">
        <xsl:attribute name="numruns">
          <xsl:value-of select="count(runid)"/>
        </xsl:attribute>
        <xsl:attribute name="maxrunlength">
          <xsl:value-of select="@maxrunlength"/>
        </xsl:attribute>
        <xsl:attribute name="minrunlength">
          <xsl:value-of select="@minrunlength"/>
        </xsl:attribute>
        <xsl:attribute name="meanrunlength">
          <xsl:value-of select="@meanrunlength"/>
        </xsl:attribute> 
        <xsl:attribute name="runlengthstd">
          <xsl:value-of select="@runlengthstd"/>
        </xsl:attribute> 
        <xsl:apply-templates select="aspectprobability" mode="rawfeature"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="featureclaimed" mode="rawfeature">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:variable name="feature" select="$features[$index]"/>
    <xsl:variable name="aspectindex" select="number(../@index)"/>
    <xsl:variable name="aspectentry" select="$feature/aspectentry[$aspectindex]"/>
    <xsl:element name="feature" namespace="http://www.cs.wisc.edu/cbi">
      <xsl:attribute name="index">
        <xsl:value-of select="@index"/>
      </xsl:attribute>
      <xsl:attribute name="probability">
        <xsl:apply-templates select="$aspectentry" mode="rawfeature"/>
      </xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="aspectentry" mode="rawfeature">
    <xsl:value-of select="@probability"/>
  </xsl:template>

  <xsl:template match="runid" mode="rawfeature">
    <xsl:variable name="index" select="@index"/>
    <xsl:variable name="run" select="$runs[position() = $index]"/>
    <xsl:element name="run" namespace="http://www.cs.wisc.edu/cbi">
      <xsl:attribute name="index">
        <xsl:value-of select="@index"/>
      </xsl:attribute>
      <xsl:attribute name="outcome">
        <xsl:value-of select="$run/@outcome"/>
      </xsl:attribute>
      <xsl:attribute name="totalcount">
        <xsl:value-of select="$run/@totalcount"/>
      </xsl:attribute>
      <xsl:attribute name="llrank">
        <xsl:value-of select="@llrank"/>
      </xsl:attribute>
      <xsl:attribute name="llvalue">
        <xsl:value-of select="@llvalue"/>
      </xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="aspectprobability" mode="rawfeature">
    <xsl:element name="aspectprobability" namespace="http://www.cs.wisc.edu/cbi">
      <xsl:copy-of select="@*"/>
    </xsl:element>
  </xsl:template> 

</xsl:stylesheet>
