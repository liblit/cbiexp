<?xml version="1.0"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>


  <xsl:variable name="projectURL" select="'http://www.cs.berkeley.edu/~liblit/sampler/'"/>


  <xsl:variable name="logo-icon">
    <a href="{$projectURL}" class="logo" title="Cooperative Bug Isolation Project">
      <img src="{$projectURL}logo.png" alt="" style="border-style: none"/>
    </a>
  </xsl:variable>


</xsl:stylesheet>
