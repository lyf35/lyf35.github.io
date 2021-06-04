---
title: 信息安全-PKI相关知识（摘录）
tags: 
- 信息安全
- PKI
categories: [理论知识, 信息安全]
sharing: True
date: 2020-12-02 14:01:15
updated: 2021-01-06 16:57:36
---

注：本文内容摘录自Public Key Infrastructure: Implementation and Design一书。



# Basics

## Definition

PKI is a framework that consists of security policies, encryption mechanisms, and applications that generate, store, and manage keys. PKI also provides procedures to generate, distribute, and utilize keys and certificates.

PKI provides a mechanism to publish the public keys that are part of public key cryptography. It describes the policies, standards, and software that are used to regulate certificates, public keys, and private keys.

<!--more-->

## Components

The components of a PKI consists:

- Certification Authority (CA)
- Registration Authority (RA)
- PKI clients
- Digital Certificates
- Certificate Distribution System or repository

### CA

The CA is a trusted third party that authenticates entities taking part in an electronic transaction. To authenticate an entity, the CA issues a digital certificate. This certificate is a digital document that establishes the credentials of the entities participating in a transaction. The digital certificates issued by CAs contain information, such as the name of the subscriber, the public and the private key of the subscriber, and the issuing CAs public key. This information depends upon the policy of the company that issues the certificates.

Before issuing a digital certificate, the CA verifies the request for a certificate with a Registration Authority (RA). For validating certificate requests, a CA uses its own procedures. These procedures depend on an organization policy and the infrastructure available to validate the request. If the request is validated, the CA issues the certificate.

### RA

An RA is responsible for the interaction between clients and CAs. Often, because of the bulk of certificate requests, it is not possible for the CA to accept certificate requests, validate the requests, and issue the certificates. In such cases, the RA acts as an intermediary between the CA and the client. RAs are especially useful for scaling PKI applications across different geographical locations. The tasks performed by the RA are given below:

- Receive entity requests and validate them
- Send requests to CA
- Receive the processed certificate from the CA
- Send the certificate to the correct entity

### PKI Clients

The entities which request CAs or RAs to issue certificates are commonly referred to as PKI Clients. To obtain a digital certificate from a CA, a PKI client needs to perform the following steps:

1. Send a request to generate a public−private key pair. A CA or the client can do this task. The key pair contains the details of the client.

2. After the key pair is generated, a request is sent to the CA for the CA certificate. This request can be routed through an RA.

3. After the client receives the certificate from the CA, it can use the certificate to identify itself as being an authenticated certificate holder.

All communication between a client and the CA is kept secure. Additionally, the client is responsible for ensuring the safety of its private key. The client can ensure the safety of your private key by using several hardware components that are available, such as tokens and smart cards.

### Digital certificates

In the PKI environment,  certificates ensure that only the public key for a certificate that has been authenticated by a certifying authority works with the private key possessed by an entity. This eliminates the chance of impersonation.

A certificate includes the following elements:

- Serial number of the certificate
- Digital signature of the CA
- Public key of the user to whom the certificate is issued
- Date of expiration
- Name of the CA that has issued the certificate

After a digital certificate is obtained, the entity can use it to communicate with recipients of information in the following manner:

1. The subscriber digitally signs the message with his or her private key to ensure message integrity and its own authenticity and sends the message to the recipient.

2. The recipient, after receiving the message, verifies the digital signature with the subscribers public key and queries the global directory database to check the validity of the subscribers digital certificate.

3. The Global directory database returns the status of the subscribers digital certificate to the recipient. The transaction is completed only if the certificate is valid.

The CA signs the digital certificates. To verify a signature, the CAs public key is needed. The public key is part of the CAs digital certificate. These certificates are typically pre−installed in Web browsers. 

After a certificate has been issued, it needs to be distributed to users and organizations. This is done by a Certificate Distribution System (CDS) or a repository.

### Certificate Distribution System (CDS) or Repository

The Certificate Distribution System (CDS) distributes certificates to users and organizations. These certificates can be distributed in two ways depending on implementation of PKI in the organization. Either the certificates can be distributed by users themselves or they can be distributed by a directory server that uses LDAP to query the user information that is stored in an X.500 compliant database. CDS distributes certificates in cooperation with the directory service server. The distribution system is used to do the following tasks:

- Generate and issue key pairs
- Certify the validity of public keys by signing the public key
- Revoke expired or lost keys
- Publish the public keys in the directory service server



# PKI Architecture

## Single CA Architecture

### Description

In this type of architecture, there is just one CA who issues and distributes certificates and Certificate Revocation Lists (CRLs) to the entities. All these entities trust this CA. In addition, these entities use only those certificates that are issued by this CA.
There are no CA trust relationships in a single CA architecture because there exists only one CA. The single CA architecture does not allow for any new CAs to be added to the PKI.

A single CA architecture presents a single point of failure. This is because there is only one CA that holds the key information for all the entities. The single CA architecture also suffers from scalability issues.

### Basic Trust List Model

In the Trust List model, PKI services are provided by a number of CAs. However, these CAs do not establish a trust relationship between them. As a result, there are no certification paths in this architecture, but only single certificates. Entities have to maintain a list of CAs that they trust. These entities use only those certificates and CRLs that have been issued by the CAs in their list of trusted CAs. 

This type of an architecture where an entity maintains a list of trusted CAs is known as Basic Trust List architecture.

### Certificate Path Construction

This architecture consists of only one CA or trust point, and as a result only one certificate can connect any subject to the trust point. In other words, it can be said that there is no path construction in a single CA architecture, and a single certificate represents the entire path.

## Enterprise PKI Architecture

### Hierarchical PKI Architecture

In this architecture, PKI services are provided by multiple CAs. Unlike the Trust List architecture, all CAs in a hierarchical PKI architecture share a trust relationship among them. The CAs in this type of architecture are connected through superior−subordinate relationships.

The CA hierarchy is an inverted tree−like structure having root at the top, referred to as root CA, which in turn contains branches or nodes. These nodes are referred to as the subordinate of the root CA. These subordinate CAs are like any other CA and perform all the functions of a CA; they can also delegate the responsibility of certificate issuance to other subordinate CAs beneath them. Whenever the root CA assigns a subordinate, the root CA will issue a certificate to the subordinate, indicating what type of work the subordinate can perform. 

The root CA usually issues certificates to subordinate CAs and not to the users. However, the subordinate CAs can issue certificates to both users and subordinate CAs at lower levels. In a hierarchical PKI, subordinate CAs do not issue certificates to their superior CA or root CA. Except for the root CA, all other CAs have a single superior CA. The issuer is the superior CA, and the subject is the subordinate CA. To add a new CA to the architecture, the root CA or any current CA issues a certificate to the new CA.

![image-20201215134308513](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105222207.png)

However, the hierarchical PKI architecture suffers from a major drawback that there is a single point of trust, the root CA, which controls the complete hierarchical PKI architecture. In case of a compromise of the root CA, the complete PKI architecture will break down.

Some important properties of hierarchical PKI architecture are:

- Scalability: Hierarchical PKIs are quite scalable
- Ease of Deployment: Being uni−directional, the hierarchical PKI architecture is quite easy to deploy
- Short Certification Path: The certification paths in a hierarchical PKI architecture are quite short.

### Mesh PKI

In the mesh PKI architecture, the CAs have a peer−to−peer relationship, rather than a superior−subordinate relationship. All CAs in a mesh PKI can be trust points, and there is no single CA around which the complete PKI architecture revolves. Since CAs issue certificates to each other, they share a bi−directional trust
relationship, and all CAs need to cross−certify each other.

There are multiple trust points in a mesh PKI architecture, and hence the compromise of a single CA cannot result in a breakdown of the complete PKI. Even if a CA is compromised, then the entities with other CAs as their trust points continue to communicate with other entities.

Since mesh contains many bidirectional relationships between the CAs, there is usually more than one certification path between any entity and a trust point. This makes path construction more complex since there are multiple choices. Some of these choices lead to a valid path while others result in useless dead−ends. In addition, mesh architecture involves construction of different certification paths by different users.

![image-20201215135157261](D:%5C%E6%8A%80%E6%9C%AF%E5%8D%9A%E5%AE%A2%5Cblog_figures%5Cimage-20201215135157261.png)

## Hybrid PKI Architecture

A hybrid architecture allows organizations with multiple PKI architectures to interoperate between themselves.  There are three types of Hybrid PKI architectures.

### Extended Trust List Architecture

In the extended trust list architecture, end entities **maintain a list of multiple trust points**. Each trust point refers to a PKI, which the entity trusts. This can be a single CA, hierarchical, or mesh PKI.

The extended Trust List generates a certificate cache. This cache consists of all the possible certification paths. Therefore, instead of constructing a certification path, you can refer to the cache and search for the appropriate path with the help of the certification path value. This value is based on the complexity of the certification path. Simple certification paths are assigned a higher value than complex paths.

### Cross−Certified Enterprise PKIs

In a cross−certified architecture, either the root CA or the subordinate CA of a particular PKI establishes a peer−to−peer trust relationship with the root CAs or subordinate CAs of another PKI.

Each user will have **a single trust point**. The cross−enterprise relationship is peer−to−peer. Once the CAs have cross−certified each other, their entities can validate entities in the other PKI.

In cross−certified PKI architecture, different users construct different certification paths for the same end entity certificate. The certification path begins at the trust point that is associated with the native PKI.

### Bridge CA Architecture

The bridge CA does not issue certificates directly to users. The bridge CA establishes peer−to−peer trust relationships with the CAs of different entities. These relationships are combined to form a bridge of trust enabling entities to interact with each other through the bridge CA. The CA that establishes a relationship with the bridge CA is referred to as the *Principal CA*. The trust relationships between a principal CA and a bridge CA are all peer−to−peer.

It is easy to add new CAs or entire enterprise PKIs to a bridge−connected PKI. The change is transparent to users as no change in trust points happens.

In a bridge CA architecture certificate path construction, there is only one cross certificate that links the native PKI to all the outside or foreign PKIs. In comparison to a mesh PKI, certification path discovery becomes easier in bridge CA architecture. The users typically know their path to the bridge CA; they need to determine only the path from the bridge CA to the
entity's certificate.



# CA Functions

## Issuing Certificates

A CA authenticates users by issuing digital certificates. These digital certificates can be issued to users, other CAs, or to both.
However, before issuing a certificate, the CA needs to verify the credentials of the certificate applicant. When a CA issues a certificate, it asserts that the certificate owner has the private key corresponding to the public key that is contained in the certificate.

While issuing a certificate, the CA digitally signs the certificates with its private key. This digital signature of the issuing CA in the certificate helps users to identify whether the CA who has issued the certificate is their trusted CA or not. To ensure the authenticity of the certificate, users can verify the signature of the CA by using the public key of the CA. It is this signature of the CA that forms the very basis of trust for certificates issued by that CA.

CAs can only issue certificates if they are requested to do so. To obtain a certificate from a CA the user needs to perform the following steps:
1. Generate his or her key pair.

2. Once a key pair has been generated, the user needs to register his or her public key with a CA.

3. Send the request for certificate issue to the RA in a predefined format.

4. The RA checks the user identification and assures itself of the identity of the user. Once done, the RA forwards the request to the CA.

5. The CA signs a certificate by using its private key and sends the certificate to the RA. The signing of the certificate binds the subscriber to his or her public key.

6. The RA forwards the certificate that it receives from the CA to the concerned user.

7. The user presents this certificate whenever desired in order to demonstrate the legitimacy of his or her key.

The certificates that are issued have to be in a specified format so that they are recognized globally. One such format that is globally recognized is the X.509 standard.

## Revoking Certificates

Every certificate has a validity period. However, some of the reasons that can result in a certificate becoming prematurely invalid are:

- Compromise of an entitys private key
- A certificate holder leaves the company
- Change in the attribute information of the entity, such as entity name, or relationship with the organization
- Compromise of the Certification Authority (CA) key

Some of the methods that a CA can use to revoke certificates are:

- Periodic Publication Mechanisms: This mechanism includes the use of Certificate Revocation Lists (CRL) and Certificate Revocation Trees (CRT).
- Online Query Mechanisms: Online Query Mechanisms comprise Online Certificate Status Protocol (OCSP) and Online Transaction Validation Protocols.

## Certificate Policy

### Definition

A certificate policy is a documented set of rules and commitments made by a CA to indicate the applicability of a certificate to a particular group of users or set of applications.

### Writing Certificate Policies

When a certificate policy is drafted, security analysts and application owners should evaluate it. This ensures that the policy covers all security aspects of a certificate and can be implemented with the given application. For writing certificate policies, you can use the RFC 2527 format. This format ensures that all the critical aspects of the certificate policy are included in a policy.

### Certificate Policies and CAs

A certificate policy is usually associated with a single CA when the CA serves as the trust point and is responsible for issuing all certificates. A certificate policy is associated with multiple CAs when a single CA is not the trust point. A CA should ensure that all subordinate CAs working on its behalf or the registration
authority comply with the certificate policies.

### Policy Object Identifiers

An OID is a simple data type defined by ANS.1 and is a sequence of integers that are separated by dots or periods.  Each OID maps to a certificate policy, which is, in turn, related to one or more CPS. Once an OID is associated with a certificate policy, you cannot make major modifications to the policy. If you make major modifications, the certificate policy needs to be associated with a new OID. As a result you can use OIDs to easily distinguish one certificate policy from another.

Certificate policies are documented and practiced by using a Certification Practice Statement (CPS). CA operations are based on its CPS, which in turn define the security policy.

## Certification Practice Statement (CPS)

### Introduction

A Certification Practice Statement (CPS) specifies how certification is practiced. That is, the statement describes how the practices that are identified in the certificate policies are implemented and enforced. A CPS specifies the policies and procedures that a company follows for issuing and managing certificates. 

Though it appears similar to certificate policies, a CPS is essentially a detailed description of the procedures implemented by an organization. The description includes any **application−specific details** that an organization might deem relevant to the document.

### Role of CAs in CPS

A CA is responsible for issuing a CPS. The tasks that a CA should perform for formulating a CPS are given below:

- Issue a CPS.
- Ensure that the RA and the subscribers are aware of the policies and CPS.
- Ensure that any CA with whom it cross−certifies complies with certificate policies and CPS that are mutually agreed. Cross−certified CAs are those CAs whose certificate policies have been evaluated and have been accepted.

### Writing a CPS

RFC 2527 format for a CPS contains a number of components for a CPS. These components are:

- Introduction
- General Provisions
- Identification and Authentication (important!)
- Operational Requirements (important!)
- Physical, Procedural, and Personnel Security Controls
- Technical Security Controls
- Certificate and CRL Profile
- Specification Administration

#### Identification and Authentication

Before a CA issues a certificate, the CA should first authenticate the certificate applicant. An applicant can be identified by the credentials presented by the applicant in the certificate application during certificate registration process. The various ways in which an applicant can be identified are

- E−mail ID of the user as given in the certificate application
- Face−to−face appearance before a CA or an RA
- Applicants unique name
- Presenting credentials to collect registered/certified mail

#### Operational Requirements

The operational requirements component of the CPS ensures that a certificate is issued or revoked by a CA only when appropriate. According to RFC 2527, operational requirements components consist of the following sections:
- Certificate Application
- Certificate Issuance
- Certificate Acceptance
- Certificate Suspension and Revocation
- Security Audit Procedures
- Records Archival
- Key Changeover
- Compromise and Disaster Recovery
- CA Termination



# Certificate Management

## Certificate Enrollment and Registration Authority

Certificate enrollment is the process by which a user sends a request for a certificate to a CA. The process involved in certificate registration might involve another authority that is different from the CA. This authority is the RA and assists a CA in performing the registration tasks. The RA can be designated by a CA or can be specific to an organization.

The RA acts as an interface between the user and the CA. Typically, the RA consists of three components: Registration authority console, Registration authority officer, and Registration authority manager. The RA typically receives requests from a user to issue certificates. These requests are made through a registration form, which is provided by the RA. The main functions of the RA include:

- Verifying the identity of a user on receiving a request
- Handling certificate enrollment
- Passing on the information to the CA to complete the registration process

## Certificate Registration

Certificate registration is the process of receiving the registration requests from users. These requests are then
processed and validated by the RA. The following steps describe the process of certificate registration:

1. The user sends a request for a registration form to a RA. The RA predefines the format of the registration form. This request for the registration form can be made online depending on the policy under which the RA functions.

2. On receiving the request, the RA sends the registration form to the user.
3. The user submits the completed registration form to the RA.
4. Based on the information in the registration form, the RA verifies the identity of the user and sends the registration request to the corresponding CA. 
5. After verifying the request, the CA sends back its response to the RA. The response can be negative if the user does not fulfill a mandatory condition.
6. If the response from the CA is positive, the RA registers the user and passes the registration information to the user.

## Key Generation

A key pair can be generated at the client−end or the CA−end. Deciding where to generate the key pair, to a large extent, depends on the availability of resources to generate the key pair and also on the intended use of the certificate.

If the key pair is generated at the users end, the public key is sent to the CA along with a request for certificate generation.

In CA Key Pair Generation, the CA signs the key pair by using a predefined cryptographic algorithm. It also involves the transfer of the generated private key to the user. To accomplish this, the CA needs to have procedures for transferring the private key safely to the owner. In addition to transferring the private key, the CA also needs to publish the certificate along with the corresponding public key in the global directory database.

The CA and the users should not only have adequate resources for generating the keys, but also have resources for performing the key backup. A backup needs to be made once the key is generated. This backup copy can be used to decrypt the messages in case the user loses the original key. The private key of the key pair should always be kept in the safe custody of the owner. This would minimize any intrusion threats to the key. The owner of the private key should also have a proper key backup facility.

## Maintaining Keys and Certificates

### Key Backup

A CA performs a key backup to ensure that if a user loses his or her private key, the data that was encrypted by using the corresponding public key can be decrypted. A key is usually backed up when the certificate is generated. Either a CA or a user can back up keys. There are no well−defined rules for backing up keys. These policies essentially depend upon the use of the keys.

### Certificate Expiry and Archiving

A CA can revoke an invalid certificate. The issuing CA, upon the receipt of the revocation request from the owner, invalidates the original certificate and enters the certificate information in a new Certificate Revocation List (CRL).

A CA must be able to establish the validity of certificates after they have expired. This is because even after the certificate has expired, all transactions that were done by using the certificate when it was valid should be permissible. Therefore, CAs should be able to maintain sufficient archival information to validate the actual identity of the person or system named in a certificate, and verify that the certificate was valid at the time the document was signed. This can be accomplished by cryptographic timestamps.

### Certificate Updates

When a certificate is due for expiration, a new public/private key is generated and the public key is associated certificate. This is referred to as key update.

To issue a new certificate, a CA issues a pair of key rollover certificates simultaneously. A description of the two keys in the key rollover certificates is given below:

- The first certificate contains the old public key and is signed with the new private key. This rollover certificate allows subscribers that have certificates signed with the new private key to construct a valid certification path to the certificates previously signed with the old private key.
- The second certificate contains the new public key and is signed with the old private key. This rollover certificate allows the subscribers with certificates signed by the old private key to construct a valid certification path to the certificates signed with the new private key.

### Issuing Multiple Certificates to Users

Different certificates that are issued to the same user or organization may be associated with different applications for different uses. If a user has multiple key pairs through multiple certificates, the user is likely to have multiple certificates because the format of the certificate usually does not allow the user to have more than a single public key. Therefore, if a single user has multiple certificates issued by a Certification Authority, the user must be ensured that each one of the certificates contains a separate public key to avoid substitution attacks.

## Certificate Retrieval and Validation

Certificate retrieval depends upon the location of a certificate. A certificate can be distributed by the means of a secure e−mail using Secure Multipurpose Internet Mail Extensions (S−MIME) or can be published in an X.500 directory. If the certificate is published in the X.500 directory, a user can then retrieve it by using Lightweight Directory Access Protocol (LDAP). LDAP enables applications running on multiple platforms to obtain directory information, such as e−mail addresses and public keys. A CA can populate an LDAP directory with a new employees details. The CA can use employee information in the LDAP directory to issue
multiple certificates simultaneously for users that match a criterion. LDAP directory can also be integrated with the Certificate Manager to automatically retrieve certificate information from the Certificate Manager and update it in the directory.

After retrieving certificates, you need to validate them. Certificate validation involves the following processes:

- Verifying that the signature is valid
- Verifying that a trusted CA has issued the certificate
- Ensuring that the certificate can be used for intended purposes
- Determining if the certificate has been revoked

## Methods of Certificate Revocation

Certificate revocation is the process of removing the validity of a certificate prematurely. There could be multiple reasons for revoking a certificate; some of the reasons are stated below:

- The certificate holder leaves the organization.
- The name and/or details of the certificate holder change.
- The issuing authority ceases to exist or the relationship between the issuing authority and the organization ceases to exist.
- There is a suspicion of private key compromise.

Certificate Revocation can be implemented in a number of ways, including:

- Periodic Publication Mechanisms: This mechanism includes the use of Certificate Revocation Lists (CRL) and Certificate Revocation Tress (CRT).
  - CRL: A CRL is a list of all certificates that have been revoked before their expiration date. CRLs are issued, maintained, and updated on a regular basis by the issuing authority or a CA.
  - CRT: Another certification method that was introduced by Valicert, a U.S.−based company. This revocation technology is based on Merkle hash trees, where the tree represents all known certificate revocation information relevant to some known set of PKI communities. The major drawback of this method is that the CRTs have to be recomputed with every update. This leads to an increase in the load on the processor, and it also reduces the timeliness of the CRT.

- Online Query Mechanisms: Online Query Mechanisms comprise Online Certificate Status Protocol (OCSP) and Online Transaction Validation Protocols.
  - OCSP: OCSP is used to obtain online revocation information about certificates. An OCSP responder is used to process the information relating to all OCSP verification requests. The OCSP responder is a trusted entity, which, on receiving the requests for the revocation information, replies to the sender of the request with status information about the online certificates.
  - Online Transaction Validation Protocols: Online Transaction Validation Protocol is used for online validation, such as business transactions through credit cards.



# PKI Management Protocols and Standards
## PKCS#10

PKCS#10, or Certificate Request Syntax Standard, defines the syntax for certificate requests. According to the PKCS#10 standard, a certificate request comprises the following three elements:

- Certificate request information: Distinguished name, Public keys and Optional attributes
- A signature algorithm identifier
- A digital signature on the certification request information

Suppose Bob requests the AllSolv CA for a certificate. Now to request for a certificate according to PKCS#10, Bob needs to
perform the following steps:

1. Generate a CertificateRequestInfo value as per the PKCS#10 specification from his public key and
   user name. As already discussed, this CertificateRequestInfo value consists of the distinguished name
   of the entity, in this case Bob, and the public key of the entity.

2. After generating the CertificateRequestInfo value, Bob needs to sign this value with his private key.
3. Finally, Bob needs to generate a CertifcateRequest value as per PKCS#10 from the CertificateRequestInfo value and his signature.

PKCS#10 is the most common standard for certificate requests. Since most of the PKI applications have become Web−enabled, certificate requests can be made online. However, PKCS#10 does not support HTTP protocols. When a certificate request is made over the Web by using HTTP, the CA is unable to authenticate the entity. Therefore, PKCS#10 is used most commonly with the Secure Sockets Layer (SSL) for making certificate requests.

A few limitations of PKCS#10 are:

- PKCS#10 is not algorithm−independent. Just like an RSA algorithm, it also assumes that the private key might be used for creating the digital signature.
- The digital signature on a PKCS#10 request does not provide all the information that a CA needs to authenticate the users. In addition, there is no well−defined mechanism to ensure that a certificate request has not been altered during its transit.
- PKCS#10 defines the syntax for only one message type, which is a certificate request, and not for the complete protocol. It does not specify the syntax and protocol for any other types of messages such as certificate revocation requests. Hence, messages other than certificate requests have to be implemented by using other protocols, such as HTTP

## PKCS#7

PKCS#7 or Cryptographic Message Syntax Standard defines the syntax for cryptographic data, such as digital signatures. PKCS allows for authentication of attribute information in addition to authenticating the content of the messages.

Some of the important uses of PKCS#7 are:

- The CAs use the PKCS#7 standard as a response to the entity requesting a certificate.
- It is used to authenticate the certificate messages sent to an entity.
- It provides complete information to the CA for processing certificate requests.
- It is used by many protocols such as S/MIME for providing security.

Let us now look at how Bob can use PKCS#7 to create a digitally signed message by using PKCS#7. Bob needs to perform the following steps to create a digitally signed message by using PKCS#7:
1. Encode a value of type ContentInfo as per PKCS#7 for the data that he wants to sign.
2. Digest the data to be signed as per PKCS#7.
3. Encode a value of type DigestInfo as per PKCS#7 from the message digest.
4. Encrypt the encoded DigestInfo with his private key.
5. Encode a SignedData type value as per PKCS#7 from the first ContentInfo value, the encrypted message digest, and other information such as his certificate and the message digest algorithm identifier.
6. Encode another ContentInfo type value from the SignedData value as per PKCS#7.

## PKCS#11

PKCS#11 was introduced to implement cryptography with the use of portable computing devices such as smart cards, PCMCIA cards, and smart diskettes as they provide a way to store the private key component of a public key/private key pair securely, under the control of a single user. 

In this protocol, the cryptographic application utilizes the device to perform the operations rather than performing cryptographic operations so that sensitive information such as private keys are never revealed. As more applications are developed for public key cryptography, a standard programming interface for these devices becomes increasingly valuable.

## PKCS#12

This is a standard, which describes the syntax for the transfer of personal identity information, including private keys, certificates, miscellaneous secrets, and extensions. Applications, browsers, and machines that support PKCS#12 allow users to import, export, and exercise a single set of personal identity information through them.

There are four combinations of privacy modes and integrity modes. The privacy modes use encryption to protect personal information from exposure, and the integrity modes protect personal information from tampering. The privacy modes are:

- Public key privacy mode: Personal information is enveloped on the source platform by using a trusted encryption public key of a known destination platform. The envelope is opened with the corresponding private key.
- Password privacy mode: Personal information is encrypted with a symmetric key derived from a user name and a privacy password. If password integrity mode is used as well, the privacy password and the integrity password may or may not be the same.

The integrity modes are:

- Public key integrity mode: Integrity is guaranteed through a digital signature on the contents of the PFX Protocol Data Unit (PDU), which is produced using the source platforms private signature key. The signature is verified on the destination platform by using the corresponding public key.
- Password integrity mode: Integrity is guaranteed through a Message Authentication Code (MAC) derived from a secret integrity password. If the password privacy mode is used as well, the privacy password and the integrity password may or may not be the same.

## Certificate Management Protocol (CMP)

Certificate Management Protocol, also known as CMP, is required for on−line interactions between different PKI entities. These entities can either be users to whom the certificates are issued or the CA that is responsible for issuing the certificates to the users. A CMP can function between two CAs for cross−certification and can also function between a user and an issuing CA. CMP also supports the participation of an RA.

The CMP is composed of four components:

- Header: This component contains the sender and recipient names, time of message, and the cryptographic algorithm that is used. Apart from this information, the header contains some optional fields such as key identifier and transaction identifier. It also contains two more fields, one for extra processing information and another for user information other than the general information field.
- Protection: This component typically contains a hashed message authentication code. It is used for securing the integrity of the header and body. This is an optional component.
- Extra certificates: This component contains extra certificates. The subscriber might require these extra certificates.
- Body: This component contains the message types.

According to CMP, there are three major components of a certificate request message:

### Certificate Request Message Format (CRMF)

CRMF comprises a request identifier, a template that contains information about the certificate, and some optional controls. The template not only consists of all fields of a certificate but also contains certificate extensions.

### Proof of possession (POP)

Proof of Possession (POP) allows an entity that is to be associated with a certificate to prove that it possesses the private key that corresponds to the public key for which a certificate request has been made.

### Registration information

Registration information consists of some supplementary information such as an entities contact information and billing information, which are used while requesting a certificate.

## Simple Certificate Enrollment Protocol

Simple Certificate Enrollment Protocol (SCEP), which was developed by Cisco Systems, is a communication protocol that is used by PKI. This protocol was designed for issuing certificates to the various network devices (by using whatever technologies were available) in a secure and scalable environment.

According to SCEP, the entities should include the following information as a prerequisite for performing any PKI activity:

- The Fully Qualified Domain Name (FQDN) of the CA or the CAs IP address
- The CA HTTP script path and the proxy information in case no direct connection to the server is available
- The URL of the certificate and CRL queries

A PKI transaction process starts with the generation of a key pair by an entity. To accomplish this, the entity can use any algorithm, such as RSA, that is supported by SCEP. Next, it obtains a CA public key using the standard HTTP Get operation after which the entity trusts the CA. After the identity of a CA has been established, the entity uses PKCS#10 to issue a certificate request and PKCS#7 to send the request to the CA. At this point the certificate enrollment process is initiated. 

SCEP supports two enrollment processes:

- Manual: In manual authentication, the entity has to wait for the CAs request until the time the CA
  manually authenticates the identity of the entity.

- Challenge password: In this type of enrollment, the entity provides a password to the CA. The CA
  authenticates the request based on this password.

After establishing the authenticity of the entity, the CA issues the certificate.

Unfortunately, SCEP suffers from the following limitations:

- It does not define messages for revocation requests. It only supports messages for certificate requests.
- It is very algorithm−specific and supports only RSA algorithm.
- It can be used only for issuing certificates to network devices. Therefore, it is very specific to an application.

## The X Series Standards

### X.500

The X.500 directory acts as the repository for certificates and for the Certificate Revocation List (CRL) into which all CAs have published their certificates. X.500 is a distributed global directory service. This means that the information in an organization can be stored locally in one or more databases referred to as Directory Service Agents (DSAs). DSAs are not specific to an organization, and one DSA can contain information about one or more organizations. Also, information about a large organization can be stored in more than one DSA. The DSA database maintains information in a hierarchy as per the X.500 information model so that information can be exchanged with other DSAs.

### X.509

X.509 is a PKI standard accepted globally by the companies involved in producing PKI products. The standard describes a hierarchical model for cross−certification of certificates from multiple CAs. X.509 certificates authenticate the identity of a person by using cryptographic mechanisms and store information and privilege about entities in the form of attributes in the certificate. The X.509 standard is supported by a number of protocols, including PEM, PKCS, S−HTTP, and SSL.

The X.509 certificates consist of the following information:

- Certificate Version: This describes the version of the certificate. The latest version is version 3. 
- Certificate serial number: The CA associates an integer with each certificate. It is unique for each certificate.
- Signature Algorithm ID: The CA signs the certificate by using an algorithm and the public key of the certificate holder.
- Issuer name: The issuer name identifies the CA that has signed and issued the certificate.
- Validity Period: This contains two dates, the date from which the certificate validity period begins and the date when the certificate validity period ends.
- Subject (user) name: The subject identifies the entity to which a private key is issued. This private key is associated with the public key stored in the subjects public key field.
- Subject public key information: This field contains the public key along with the identifier of the algorithm with which the key is used.
- Digital Signature: It contains the digital signature (signature with the private key) of the issuer.

Although the information that is contained in the certificate identifies the entity, it does not guarantee the uniqueness of the subject and issuing CA name. There can be a situation when a single subject name can be associated with two entities. To help resolve such a situation, X.509 version 2 was introduced. X.509 version 2 is the next version of X.509, which, in addition to the preceding information, contains two new fields:

- Issuer unique identifier: This field contains a unique ID of the certificate issuer. Although the issuer name field also exists, this field ensures that the identifier is unique to the issuing CA and doesn't identify some other CA.
- Subject unique identifier: This field is similar to the issuer unique identifier. This field ensures the uniqueness of the subject of the certificate.

X.509 introduced the concept of extensions. This is the latest version and was introduced in 1996. Extensions are fields that contain information in addition to the fields available in earlier versions. By using X.509 version 3 extensions, an organization can define certificate content, depending on their requirements.

- Alternate Name: An alternate name can be an e−mail address or any other name that is created by an entity or an organization to identify the entity in a user−friendly manner. In addition, the use of alternate names in X.509 version 3 certificates helps to identify the entity without needing to refer to the X.500 directory.
- Subject Directory Attributes: This extension can contain the attribute values of an entity, in addition to the subjects name, as defined in the X.500 directory.

- Certificate Policies: The CA can include the policies based on which the certificates are issued. These policies indicate the usability of a certificate under different conditions. An associated object identifier that is referred to as policy OID identifies each policy. Policy mapping is used in situations when more than one CA exists in a hierarchy. In order for the policy of one CA to be recognized by the entity of another CA, the policy of the first CA should be mapped with the policy of the second CA. The policy mapping extension facilitates the inclusion of policy mapping information.
- Certification Path Constraint: The Certification Path Constraint extension was introduced to restrict the certification path in a trust model. A constraint is a kind of condition applied as a restriction to the certification path. To understand certification path constraint better, lets analyze the following example.
