<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>

  <xsl:output
    method="html"
    />


  <xsl:variable name="preds" select="document('predictor-info.xml')/predictor-info/info"/>

  <xsl:template match="/">
    <html>
      <body>
        <table>
          <xsl:apply-templates/>
        </table>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="exp">
    <xsl:variable name="index" select="@index"/>
    <xsl:variable name="lineno" select="@lineno"/>
    <tr>
      <th><xsl:value-of select="$lineno"/></th>
      <td><xsl:value-of select="$index"/></td>
      <xsl:apply-templates select="$preds[position() = $index]" mode="static-cells"/>
    </tr>
  </xsl:template>

</xsl:stylesheet>
