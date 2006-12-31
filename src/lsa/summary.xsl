<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

<xsl:output method="html"/>

  <xsl:template match="/">
    <html>
      <head>
      </head>
      <body>
        <h2>Aspects:</h2>
        <h3>Usage:</h3>
        <table class="aspecttable">
          <xsl:call-template name="aspectheader"/>
          <xsl:for-each select="plsa/aspect[@kind='usage']">
            <xsl:call-template name="aspectrow"/>
          </xsl:for-each>
        </table>
        <h3>Buggy:</h3>
        <table class="aspecttable">
          <xsl:call-template name="aspectheader"/>
          <xsl:for-each select="plsa/aspect[@kind='bug']">
            <xsl:call-template name="aspectrow"/>
          </xsl:for-each>
        </table>
        <h3>Failing Runs:</h3>
        <xsl:variable name="failing" select="/plsa/run[@outcome='failure']"/>
        <table class="failingruns">
        <tr>
        <td>Total:</td>
        <td><xsl:value-of select="count($failing)"/></td>
        </tr>
        <tr>
        <td>Claimed by some buggy aspect:</td>
        <td><xsl:value-of select="count(/plsa/aspect[@kind='bug']/runid)"/></td>
        </tr>
        </table>
        <table class="cotable">
          <xsl:call-template name="cotableheader"/>  
          <xsl:for-each select="plsa/aspect[@kind='bug']">
            <xsl:call-template name="cotablerow"/>
          </xsl:for-each>
        </table> 
      </body>
    </html>
  </xsl:template>

  <xsl:template name="aspectheader">
    <tr>
      <th>Aspect Number</th>
      <th>Num Runs(Total/Succeeding/Failing)</th>
      <th>Runs</th>
      <th>Features</th>
    </tr>
  </xsl:template>

  <xsl:template name="aspectrow">
    <xsl:variable name="runs" select="id(runid/@idref)"/>
    <xsl:variable name="succeeding" select="count($runs[@outcome='success'])"/>
    <xsl:variable name="failing" select="count($runs[@outcome='failure'])"/>
    <tr>
      <td><xsl:value-of select="@index"/></td>
      <td>
        (<xsl:value-of select="count($runs)"/>/<xsl:value-of select="$succeeding"/>/<xsl:value-of select="$failing"/>)
      </td>
      <td></td>
      <td></td>
    </tr>
  </xsl:template>

  <xsl:template name="cotableheader">
    <tr>
      <th>Aspect</th>
      <xsl:for-each select="plsa/aspect[@kind='usage']">
        <th><xsl:value-of select="@index"/></th>
      </xsl:for-each>
      <th>Total</th>
    </tr>
  </xsl:template>

  <xsl:template name="cotablerow">
    <xsl:variable name="runs" select="id(runid/@idref)"/>
    <tr>
      <td><xsl:value-of select="@index"/></td>
      <xsl:for-each select="/plsa/aspect[@kind='usage']">
        <xsl:call-template name="cotableentry">
          <xsl:with-param name="runs" select="$runs"/>
        </xsl:call-template>
      </xsl:for-each>
      <td><xsl:value-of select="count($runs)"/></td> 
    </tr>
  </xsl:template> 

  <xsl:template name="cotableentry">
    <xsl:param name="runs"/>
    <xsl:variable name="usageid" select="@id"/>
    <td>
      <xsl:value-of select="count($runs/usageaspect[@idref = $usageid])"/>
    </td>
  </xsl:template>

</xsl:stylesheet>
