<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="projected-view.xsl"/>
  <xsl:import href="moss-view-exports.xsl"/>

  <xsl:template mode="static-headings" match="view">
    <xsl:apply-imports/>
    <xsl:call-template name="extra-moss-headings"/>
  </xsl:template>

  <xsl:template mode="static-cells" match="predictor">
    <xsl:apply-imports/>
    <xsl:apply-templates mode="extra-moss-cells" select="."/>
  </xsl:template>

</xsl:stylesheet>
