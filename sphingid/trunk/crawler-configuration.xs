<xs:schema attributeFormDefault="unqualified" elementFormDefault="qualified" xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="crawlerConfiguration">
    <xs:complexType mixed="true">
      <xs:sequence>
        <xs:element type="xs:string" name="crawlerName"/>
        <xs:element name="timeSettings">
          <xs:complexType>
            <xs:sequence>
              <xs:element type="xs:float" name="defaultCrawlDelay"/>
              <xs:element type="xs:float" name="maxCrawlDelay"/>
              <xs:element type="xs:float" name="defaultRecrawlInterval"/>
              <xs:element type="xs:float" name="minimumRecrawlInterval"/>
              <xs:element type="xs:float" name="recrawlMultiplier"/>
              <xs:element type="xs:float" name="connectionTimeout"/>
              <xs:element type="xs:float" name="readTimeout"/>
              <xs:element type="xs:float" name="displayStatisticsInterval"/>
              <xs:element type="xs:float" name="crawlStateWriteInterval"/>
            </xs:sequence>
          </xs:complexType>
        </xs:element>
        <xs:element name="urlFilters">
          <xs:complexType>
            <xs:sequence>
              <xs:element type="xs:string" name="disallow" maxOccurs="unbounded" minOccurs="0"/>
            </xs:sequence>
          </xs:complexType>
        </xs:element>
        <xs:element name="seedUrls">
          <xs:complexType>
            <xs:sequence>
              <xs:element name="seedUrl" maxOccurs="unbounded" minOccurs="0">
                <xs:complexType>
                  <xs:sequence>
                    <xs:element type="xs:anyURI" name="url"/>
                    <xs:element type="xs:string" name="customUrlFilters"/>
                  </xs:sequence>
                </xs:complexType>
              </xs:element>
            </xs:sequence>
          </xs:complexType>
        </xs:element>
        <xs:element name="paths">
          <xs:complexType>
            <xs:sequence>
              <xs:element type="xs:string" name="crawlDatabase"/>
              <xs:element type="xs:string" name="extractedDocuments"/>
            </xs:sequence>
          </xs:complexType>
        </xs:element>
        <xs:element type="xs:float" name="similarityThreshold"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
</xs:schema>