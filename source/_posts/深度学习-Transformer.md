---
title: 深度学习-Transformer
tags: 
- 深度学习
categories: [理论知识, 深度学习]
sharing: True
date: 2021-07-01 19:51:34
updated: 2021-07-12 22:58:19
---



# 简介

## Transformer的来历

Transformer是Attention is All You Need论文中提出的一种新的框架。在Transformer结构中，抛弃了传统的CNN和RNN，整个网络的结构仅由Self-attention和全连接的前馈网络组成。

由于Attention机制最初是基于seq2seq提出的，因此下文将首先简单地介绍seq2seq和Attention机制，然后再详细介绍Transformer。

<!--more-->

## Seq2Seq

Seq2Seq属于Encoder-Decoder结构中的一种，它的基本思想是使用两个RNN（或者LSTM、GRU等），一个作为Encoder，另一个作为Decoder。Encoder负责将输入序列压缩成指定长度的向量，这个向量就可以看作是这个序列的语义，这一过程称为编码。而Decoder则负责根据语义向量生成指定的序列，这个过程也被称为解码。

下图所示便是使用RNN构造的一个Encoder-Decoder结构，它可以被用于机器翻译等应用中去：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210703222058.io%2526app%253D2002%2526size%253Df9999%252C10000%2526q%253Da80%2526n%253D0%2526g%253D0n%2526fmt%253Djpeg" alt="img" style="zoom:67%;" />

## Attention

在Seq2Seq架构中，Encoder要将整个输入序列的信息全部编码成一个向量传给Decoder。这样就要求了编码向量中包含有原始序列中尽可能多的信息。而当要编码的序列较长时，由于编码向量的大小固定，因此这个向量可能无法保存有序列的所有信息，从而导致模型精度的下降。

为了解决这一问题，人们在Seq2Seq架构中引入了Attention机制。从感性的角度来看，当我们在看一张图片时，我们可能更加关注图片内的文字和人像，忽略掉图片中的背景部分；与之类似的是，我们在读一句话的时候，也会更加关注于其中关键性的名词和动词，而忽略掉一些无关紧要的介词、形容词等。而Attention机制便类似于人的这一机制。

具体来说，Attention机制通过保留Encoder对于输入序列的中间输出结果，然后结合Decoder的输入值以及Encoder的这些中间输出结果，计算得到一个对应的注意力输出值，并且在解码器输出时，将输出序列与注意力值关联。如下图所示：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210703222104.png" alt="image-20210701215504394" style="zoom:67%;" />

# Transformer

## 基本原理

Transformer是Google在2017年提出的用于机器翻译的模型，在它的内部，其实是一个Encoder-Decoder的结构。在Transformer中，抛弃了传统的CNN和RNN，整个网络结构完全由Attention机制组成，并且采用了6层的Encoder-Decoder结构。

以一个简单的例子说明Transformer的结构及其原理：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210703222109" alt="图片" style="zoom: 67%;" />

其中，左边的部分为Encoder，右边部分为Decoder，原论文中使用的Transformer结构中，N=6。编码器负责将自然语言序列映射称为一个隐藏层，即一个包含了自然语言序列信息的数学表达；而解码器则将隐藏层再映射为自然语言序列。

例如我们以Why do we work？作为输入，Transformer的工作流程如下：

1. 输入自然语言序列Why do we work？到编码器中；
2. 编码器会负责将这句话编码为一个向量，这个向量将作为解码器的输入；
3. 输入一个起始符号\<start\>给解码器，解码器便可以生成第一个输出；
4. 然后将第一个输出作为输入继续输入解码器，便可以得到第二个输出；
5. 以此类推，直到解码器输出\<end\>，代表序列生成完成。

## 结构解析

由于编码器与解码器的结构类似，因此下面我们以编码器为例，说明Transformer的结构。为了叙述方便，此处仍然使用上面的Why do we work为例来说明其原理。

### 词嵌入

由于模型无法直接对输入的单词进行解析，因此第一步要做的事情就是词嵌入（图中的Input Embedding），就是将输入数据中的每一个词都编码为一个词向量。

词嵌入可以使用Word2Vec的方法。为了叙述方便，我们以Why do we work这句话为例，并假设使用的Word2Vec将一个单词映射为一个512长度的词向量。那么输入数据$X$就是一个维度为[4]的向量，在经过词嵌入之后，得到一个维度为[4,512]的向量。

### 位置嵌入

文字的位置信息很重要，在上图的Encoder结构中，并没有用到类似于RNN的循环结构，因此Encoder本身无法捕捉顺序序列。为了加入位置信息，Transformer使用了位置嵌入（即图中的Positional Encoding）的方法。具体来讲，Transformer使用了sin-cos规则，利用正弦和余弦函数的周期性变化来向模型提供位置信息：
$$
PE_{\text{pos},2i}=\sin(\text{pos}/(10000^{2i/d_{\text{model}}})) \\
PE_{\text{pos},2i+1}=\cos(\text{pos}/(10000^{2i/d_{\text{model}}})) \\
$$
其中，$\text{pos}$指的是句子中词的位置，例如we对应的$\text{pos}$值为2，do对应的pos值为1；$i$指的是字嵌入的维度，取值范围为$[0,512)$；$d_{model}$指的是词嵌入向量的长度，即512。

也就是说，词嵌入向量的512个位置对应于512个不同的三角函数公式，产生不同的周期性变化。在词向量不同的位置处，这512个三角函数值也有所不同，也就对应了独特的位置嵌入向量。

接下来，将每个词向量与它的位置嵌入向量相加，便得到了下一层的输入$X_{\text{embedding}}$。

### 多头注意力层

图中的Multi-Head Attention主要包括两个重要的点，一个是自注意力（self-attention）机制，另外一个是多头（Multi-Head）机制。

首先介绍自注意力机制。自注意力模块的结构如下：

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210703222120.png)

其中，$Q$，$K$，$V$对应于三个不同的矩阵，各自又被称为查询向量、键向量和值向量，是通过对$X_{\text{embedding}}$做线性变换而来的。三个矩阵$Q$，$K$，$V$对应于三个不同的权值矩阵$W^Q$，$W^K$和$W^V$，而$Q$，$K$，$V$通过下面的公式计算而得：
$$
Q=X_{\text{embedding}} W^Q \\
K=X_{\text{embedding}} W^K \\
V=X_{\text{embedding}} W^V
$$
我们假设三个权值矩阵的维度都为$512\times 64$，那么$Q$，$K$，$V$的维度相应为$4\times 64$。

而最终的Attention可以通过如下公式计算而得：
$$
\text{Attention}(Q,K,V)=\text{softmax}\left(\frac{QK^T}{\sqrt{d_k}}\right)V
$$
其中$d_k$代表$Q$，$K$，$V$的维数，即64，这一参数用于稳定梯度的值。因此最终计算出的Attention也是一个$4\times 64$的矩阵。

而“多头”机制其实相当于是有多个$Q$，$K$，$V$的集合，即多个Self-Attention的集成。我们假设head的个数为8，那么将$X_{\text{embedding}}$输入到这8个Self-Attention模块之后，便会得到相对应的输出。将它们按列拼接起来再送到一个全连接层，便可以得到Multi-Head Attention模块的输出。

在Multi-Head Attention模块中，多个$Q$，$K$，$V$的集合是通过将原始的$Q$，$K$，$V$通过不同的全连接层进行线性变换，得到多组$Q$，$K$，$V$的值。

综上，Multi-Head Attention模块可以表示为下图所示的结构：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210703222124.png" alt="image-20210703003709815" style="zoom:67%;" />

### 残差链接和标准化

Transformer结构图中的Add&Norm模块便对应于残差链接和标准化操作。通过合理地设置Multi-Head Attention模块中每个子模块的维度，可以使得模块的输出维度与$X_{\text{embedding}}$的维度完全一致，这样便可以将他们直接进行元素相加，从而实现残差链接。

而标准化则是将残差链接的结果做BatchNorm操作进行批标准化。批标准化的原理此处不再赘述，可参考本博客中介绍深度前馈网络的文章。

### 前向网络

Transformer结构图中的FeedForward模块其实指的是一个前向网络。在原文中是由两个线性变换模块，以及这两个模块中间的ReLU激活函数组成。这一模块输入和输出的维度相等。

### Decoder的变化

在Decoder中，Multi-Head Attention模块与Encoder有一些不同，下面进行详细说明。

Decoder中Multi-Head Attention模块的形式与Encoder一致，唯一不同的是其$Q$，$K$，$V$矩阵的来源。在Decoder中，$Q$矩阵来自于下面子模块的输出，而$K$和$V$矩阵则来源于整个Encoder的输出。

而由于Decoder的目的是进行预测，它看不到未来的序列。所以Decoder中的Masked Multi-Head Attention模块需要将当前预测的单词以及之后的单词全部掩盖掉。

## 批处理

在上面说明Transformer结构的时候，我们以一句话作为输入来举例讲解其原理。Transformer支持计算多句话组成的batch，在上面的推导过程中，只需要在每个输入、中间计算结果和输出的维度中，再添加一个batchsize作为第0个维度，便得到它们在批量处理时的维度。

此外，由于每个句子的长度不一样，因此在计算时需要按照最长的句子长度统一处理。对于短句，则可以进行填充操作，从而使得它们的长度对齐。



# 代码示例

下面的代码是使用Transformer训练一个对话机器人的代码，主要参考了https://pytorch.org/tutorials/beginner/translation_transformer.html和https://pytorch.org/tutorials/beginner/chatbot_tutorial.html?highlight=chatbot这两个官方教程。

```python
import math
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torchtext.data.utils import get_tokenizer
from torchtext.vocab import build_vocab_from_iterator
from typing import Iterable, List
from torch.nn.utils.rnn import pad_sequence
import os
import codecs
import re
import csv
import unicodedata
import itertools
```


```python
DEVICE = 'cuda' if torch.cuda.is_available() else 'cpu'
batch_size = 32
```

下面的操作是清洗掉原始数据无用的部分，只保留对话


```python
corpus_name = "cornell movie-dialogs corpus"
corpus = os.path.join('./', corpus_name)
```


```python
# Split each line of the file into a dictionary of fields (lineID, characterID, movieID, character, text)
# The content of file "movie_lines.txt" is as follows:
# L1045 +++$+++ u0 +++$+++ m0 +++$+++ BIANCA +++$+++ They do not!
# L1044 +++$+++ u2 +++$+++ m0 +++$+++ CAMERON +++$+++ They do to!
def loadlines(filename, fields):
    lines = {}
    with open(filename, 'r', encoding='iso-8859-1') as f:
        for line in f:
            values = line.split(" +++$+++ ")
            lineobj = {}
            for i, field in enumerate(fields):
                lineobj[field] = values[i]
            lines[lineobj['lineID']] = lineobj
    return lines
```


```python
# Group fields of lines into conversations. fields = ['character1ID', 'character2ID', 'movieID', 'utteranceIDs']
# The content of file "movie_conversations.txt" is as follows:
# u0 +++$+++ u2 +++$+++ m0 +++$+++ ['L194', 'L195', 'L196', 'L197']
# u0 +++$+++ u2 +++$+++ m0 +++$+++ ['L198', 'L199']
def loadconversations(filename, lines, fields):
    conversations = []
    with open(filename, 'r', encoding='iso-8859-1') as f:
        for line in f:
            values = line.split(" +++$+++ ")
            convObj = {}
            for i, field in enumerate(fields):
                convObj[field] = values[i]
            utterance_id_pattern = re.compile('L[0-9]+')
            lineIDs = utterance_id_pattern.findall(convObj['utteranceIDs'])
            convObj['lines']=[]
            for lineID in lineIDs:
                convObj['lines'].append(lines[lineID])
            conversations.append(convObj)
    return conversations
```


```python
# Extract pairs of sentences from conversations
def extractSentencePairs(conversations):
    qa_pairs = []
    for conversation in conversations:
        for i in range(len(conversation['lines'])-1):
            inputline = conversation['lines'][i]['text'].strip()
            targetline = conversation['lines'][i+1]['text'].strip()
            if inputline and targetline:
                qa_pairs.append([inputline, targetline])
    return qa_pairs
```


```python
datafile = './cornell movie-dialogs corpus/formatted_movie_lines.txt'
delimiter = '\t'
delimiter = str(codecs.decode(delimiter, 'unicode_escape'))
```


```python
lines = {}
conversations = []
MOVIE_LINES_FIELDS = ['lineID', 'characterID', 'movieID', 'character', 'text']
MOVIE_CONVERSATIONS_FIELDS = ['character1ID', 'character2ID', 'movieID', 'utteranceIDs']
```


```python
lines = loadlines('./cornell movie-dialogs corpus/movie_lines.txt', MOVIE_LINES_FIELDS)
```


```python
conversations = loadconversations('./cornell movie-dialogs corpus/movie_conversations.txt', lines, MOVIE_CONVERSATIONS_FIELDS)
```

```python
with open(datafile, 'w', encoding='utf-8') as outputfile:
    writer = csv.writer(outputfile, delimiter=delimiter, lineterminator='\n')
    for pair in extractSentencePairs(conversations):
        writer.writerow(pair)
```

通过上面的操作，我们得到了用于训练的语料库，其中每一条都代表一问一答组成的对话。接下来需要将它们转为数字表示的格式


```python
PAD_token = 0
SOS_token = 1
EOS_token = 2

# 词库的定义。语料库中所有的词语及其映射到的整数都保存在类的成员变量中。
class Voc:
    def __init__(self, name):
        self.name = name
        self.trimmed = False
        self.word2index = {'PAD': PAD_token, 'SOS': SOS_token, 'EOS': EOS_token}
        self.word2count = {}
        self.index2word = {PAD_token: 'PAD', SOS_token: 'SOS', EOS_token: 'EOS'}
        self.num_words = 3
        
    def addSentence(self, sentence):
        for word in sentence.split(' '):
            self.addWord(word)
    
    def addWord(self, word):
        if word not in self.word2index:
            self.word2index[word] = self.num_words
            self.word2count[word] = 1
            self.index2word[self.num_words] = word
            self.num_words += 1
        else:
            self.word2count[word] += 1
    
    def trim(self, min_count):
        if self.trimmed:
            return
        self.trimmed = True
        
        keep_words = []
        
        for k, v in self.word2count.items():
            if v >= min_count:
                keep_words.append(k)
                
        self.word2index = {'PAD': PAD_token, 'SOS': SOS_token, 'EOS': EOS_token}
        self.word2count = {}
        self.index2word = {PAD_token: 'PAD', SOS_token: 'SOS', EOS_token: 'EOS'}
        self.num_words = 3
        
        for word in keep_words:
            self.addWord(word)
```

下面的函数是对语料库进行清洗，去除掉其中过短或者过长的语句，并在此过程中同时建立词库


```python
MAX_LENGTH = 15
def unicodeToAscii(s):
    return ''.join(
        c for c in unicodedata.normalize('NFD', s)
        if unicodedata.category(c) != 'Mn'
    )

# Lowercase, trim and remove non-letter characters
def normalizeString(s):
    s = unicodeToAscii(s.lower().strip())
    s = re.sub(r"([.!?])", r" \1", s)
    s = re.sub(r"[^a-zA-Z.!?]+", r" ", s)
    s = re.sub(r"\s+", r" ", s).strip()
    return s

def readVocs(datafile, corpus_name):
    lines = open(datafile, encoding='utf-8').read().strip().split('\n')
    pairs = [[normalizeString(s) for s in l.split('\t')] for l in lines]
    voc = Voc(corpus_name)
    return voc, pairs

def filterPair(p):
    return len(p[0].split(' ')) < MAX_LENGTH and len(p[1].split(' ')) < MAX_LENGTH

def filterPairs(pairs):
    return [pair for pair in pairs if filterPair(pair)]

def loadPrepareData(corpus, corpus_name, datafile):
    voc, pairs = readVocs(datafile, corpus_name)
    pairs = filterPairs(pairs)
    for pair in pairs:
        voc.addSentence(pair[0])
        voc.addSentence(pair[1])
    return voc, pairs

voc, pairs = loadPrepareData(corpus, corpus_name, datafile)
```


```python
MIN_COUNT = 3

def trimRareWords(voc, pairs, MIN_COUNT):
    voc.trim(MIN_COUNT)
    keep_pairs = []
    for pair in pairs:
        if len(pair) != 2:
            continue
        input_sentence = pair[0]
        output_sentence = pair[1]
        keep_input = True
        keep_output = True
        
        for word in input_sentence.split(' '):
            if word not in voc.word2index:
                keep_input = False
                break
                
        for word in output_sentence.split(' '):
            if word not in voc.word2index:
                keep_output = False
                break
                
        if keep_input and keep_output:
            keep_pairs.append(pair)
            
    return keep_pairs
```


```python
pairs = trimRareWords(voc, pairs, MIN_COUNT)
```

下面的函数是使用词库构造数据集的函数


```python
def indexesFromSentence(sentence, voc=voc):
    return [voc.word2index[word] for word in sentence.split(' ')]

def sequentialTransforms(*transforms):
    def func(txt_input):
        for transform in transforms:
            txt_input = transform(txt_input)
        return txt_input
    return func

def tensorTransform(token_ids):
    return torch.cat((torch.tensor([SOS_token]),
                 torch.tensor(token_ids),
                 torch.tensor([EOS_token])))
text_transform=sequentialTransforms(indexesFromSentence, tensorTransform)

def collate_fn(batch):
    source_batch = []
    target_batch = []
    for source_sample, target_sample in batch:
        source_batch.append(text_transform(source_sample))
        target_batch.append(text_transform(target_sample))
    source_batch = pad_sequence(source_batch, padding_value=PAD_token)
    target_batch = pad_sequence(target_batch, padding_value=PAD_token)
    return source_batch, target_batch
```


```python
# Transfromer中的位置编码操作
class PositionalEncoding(nn.Module):
    def __init__(self, embedding_size, maxlen=5000):
        super(PositionalEncoding, self).__init__()
        den = torch.exp(-torch.arange(0, embedding_size, 2)*math.log(10000) / embedding_size)
        pos = torch.arange(0, maxlen).reshape(maxlen, 1)
        pos_embedding = torch.zeros((maxlen, embedding_size))
        pos_embedding[:, 0::2] = torch.sin(pos*den)
        pos_embedding[:, 1::2] = torch.cos(pos*den)
        pos_embedding = pos_embedding.unsqueeze(-2)
        
        self.register_buffer('pos_embedding', pos_embedding)
        
    def forward(self, token_embedding):
        return token_embedding+self.pos_embedding[:token_embedding.size(0), :]
```


```python
# Word2Vec操作
class TokenEmbedding(nn.Module):
    def __init__(self, vocab_size, embedding_size):
        super(TokenEmbedding, self).__init__()
        self.embedding = nn.Embedding(vocab_size, embedding_size)
        self.embedding_size = embedding_size
    def forward(self, tokens):
        return self.embedding(tokens.long())
```

备注—PyTorch的`TransformerEncoder`和`TransformerDecoder`两个模块中，Masking的用法：

mask分为两种，一种是`*_mask`，在这种mask中，没有被掩盖的部分的值为0.0，而被掩盖的部分则值为-inf；另一种mask为`*_key_padding_mask`，在这种mask中，使用True代表这个位置被掩盖，而False代表未被掩盖。


```python
class Seq2SeqTransformer(nn.Module):
    def __init__(self,
            source_vocab_size,
            target_vocab_size,
            num_encoder_layers = 3,
            num_decoder_layers = 3,
            embedding_size = 512,
            nhead = 8,
            dim_feedforward = 512,
            dropout = 0.1):
        super(Seq2SeqTransformer, self).__init__()
        self.transformer = nn.Transformer(d_model=embedding_size,
                              nhead=nhead,
                              num_encoder_layers=num_encoder_layers,
                              num_decoder_layers=num_decoder_layers,
                              dim_feedforward=dim_feedforward)
        self.generator = nn.Linear(embedding_size, target_vocab_size)
        self.source_token_embedding = TokenEmbedding(source_vocab_size, embedding_size)
        self.target_token_embedding = TokenEmbedding(target_vocab_size, embedding_size)
        self.positional_encoding = PositionalEncoding(embedding_size)
        
    def _generate_square_subsequent_mask(self, sz):
        mask = (torch.triu(torch.ones(sz, sz)) == 1).transpose(0, 1)
        mask = mask.float().masked_fill(mask == 0, float('-inf')).masked_fill(mask == 1, float(0.0))
        return mask
    
    def create_mask(self, source, target):
        source_seq_len = source.shape[0]
        target_seq_len = target.shape[0]
        
        target_mask = self._generate_square_subsequent_mask(target_seq_len)
        source_mask = torch.zeros((source_seq_len, source_seq_len), device=DEVICE).type(torch.bool)
        
        source_padding_mask = (source == PAD_token).transpose(0,1)
        target_padding_mask = (target == PAD_token).transpose(0,1)
        
        return source_mask, target_mask, source_padding_mask, target_padding_mask
        
    def forward(self, 
            source, 
            target):
        source_mask, target_mask, source_padding_mask, target_padding_mask = self.create_mask(source, target[:-1,:])
        source_mask = source_mask.to(DEVICE)
        target_mask = target_mask.to(DEVICE)
        source_padding_mask = source_padding_mask.to(DEVICE)
        target_padding_mask = target_padding_mask.to(DEVICE)
        source_embedding = self.positional_encoding(self.source_token_embedding(source))
        target_embedding = self.positional_encoding(self.target_token_embedding(target[:-1,:]))
        
        outs=self.transformer(source_embedding, 
                       target_embedding, 
                       source_mask, 
                       target_mask,
                       None,
                       source_padding_mask,
                       target_padding_mask)
        return self.generator(outs)
    
    def encode(self, source, source_mask):
        return self.transformer.encoder(self.positional_encoding(self.source_token_embedding(source)),source_mask)
    
    def decode(self, target, memory, target_mask):
        return self.transformer.decoder(self.positional_encoding(self.target_token_embedding(target)), memory, target_mask)
```


```python
torch.manual_seed(0)
vocab_size = voc.num_words
transformer = Seq2SeqTransformer(vocab_size, vocab_size)
transformer = transformer.to(DEVICE)
loss_fn = torch.nn.CrossEntropyLoss(ignore_index=PAD_token)
optimizer = torch.optim.Adam(transformer.parameters(), lr=0.0001)
```


```python
train_size = int(len(pairs)*0.8)
val_size = int(len(pairs)*0.2)
```


```python
train_dataloader = DataLoader(pairs[:train_size], batch_size=batch_size, collate_fn=collate_fn)
val_dataloader = DataLoader(pairs[train_size:], batch_size=batch_size, collate_fn=collate_fn)
```


```python
def train_epoch(model, dataloader, optimizer):
    model.train()
    losses = 0
    
    for source, target in dataloader:
        source = source.to(DEVICE)
        target = target.to(DEVICE)
        
        logits = model(source, target)

        optimizer.zero_grad()
        
        target_out=target[1:,:]

        loss = loss_fn(logits.reshape(-1, logits.shape[-1]), target_out.reshape(-1)) #需要注意传入参数的顺序
        loss.backward()

        optimizer.step()
        losses += loss.item()

    return losses / len(dataloader)


def evaluate(model, dataloader):
    model.eval()
    losses = 0

    for source, target in dataloader:
        source = source.to(DEVICE)
        target = target.to(DEVICE)
        
        logits = model(source, target)
        
        target_out=target[1:,:]

        loss = loss_fn(logits.reshape(-1, logits.shape[-1]), target_out.reshape(-1))
        losses += loss.item()

    return losses / len(dataloader)
```


```python
epoches = 10

for epoch in range(epoches):
    train_loss = train_epoch(transformer, train_dataloader, optimizer)
    val_loss = evaluate(transformer, val_dataloader)
    print((f"Epoch: {epoch}, Train loss: {train_loss:.3f}, Val loss: {val_loss:.3f}"))
```

    Epoch: 0, Train loss: 4.303, Val loss: 4.004
    Epoch: 1, Train loss: 3.832, Val loss: 3.882
    Epoch: 2, Train loss: 3.669, Val loss: 3.824
    Epoch: 3, Train loss: 3.545, Val loss: 3.806
    Epoch: 4, Train loss: 3.438, Val loss: 3.801
    Epoch: 5, Train loss: 3.340, Val loss: 3.804
    Epoch: 6, Train loss: 3.245, Val loss: 3.823
    Epoch: 7, Train loss: 3.151, Val loss: 3.839
    Epoch: 8, Train loss: 3.060, Val loss: 3.866
    Epoch: 9, Train loss: 2.967, Val loss: 3.903

训练完成之后，便可以使用训练好的Transformer来自动生成对话：

```python
def greedyDecode(model, src, src_mask, max_len, start_symbol):
    src = src.to(DEVICE)
    src_mask = src_mask.to(DEVICE)

    memory = model.encode(src, src_mask)
    ys = torch.ones(1, 1).fill_(start_symbol).type(torch.long).to(DEVICE)
    memory = memory.to(DEVICE)
    for i in range(max_len-1):
        tgt_mask = (model._generate_square_subsequent_mask(ys.size(0)).type(torch.bool)).to(DEVICE)
        out = model.decode(ys, memory, tgt_mask)
        out = out.transpose(0, 1)
        prob = model.generator(out[:, -1])
        _, next_word = torch.max(prob, dim=1)
        next_word = next_word.item()

        ys = torch.cat([ys, torch.ones(1, 1).type_as(src.data).fill_(next_word)], dim=0)
        if next_word == EOS_token:
            break
    return ys

def getSentenceFromTokens(tokens, voc=voc):
    sentence = ""
    for token in tokens:
        sentence += voc.index2word[token]
        sentence += ' '
    return sentence

def getAnswer(model, src_sentence):
    model.eval()
    src_sentence = normalizeString(src_sentence)
    src = text_transform(src_sentence).view(-1, 1)
    num_tokens = src.shape[0]
    src_mask = (torch.zeros(num_tokens, num_tokens)).type(torch.bool)
    tgt_tokens = greedyDecode(model, src, src_mask, max_len=MAX_LENGTH, start_symbol=SOS_token).flatten()
    return getSentenceFromTokens(list(tgt_tokens.cpu().numpy())).replace("SOS", "").replace("EOS", "")
```


```python
getAnswer(transformer, "Do you have time tomorrow?")
```


    ' i m not sure .  '


```python
getAnswer(transformer, "The movie looks great .")
```


    ' i m sorry .  '


```python
getAnswer(transformer, "Where are you ?")
```


    ' i m here .  '

从中可以看出，由于在训练模型时使用的数据集较为简单，因此有一些问题的回答与输入不太能对得上。



# 参考

1. [[1706.03762\] Attention Is All You Need (arxiv.org)](https://arxiv.org/abs/1706.03762)
2. [Seq2Seq模型概述 - 简书 (jianshu.com)](https://www.jianshu.com/p/b2b95f945a98)
3. [深度学习中 的 Attention机制_GerHard 的博客-CSDN博客_attention机制](https://blog.csdn.net/guohao_zhang/article/details/79540014)
4. [搞懂Transformer结构，看这篇PyTorch实现就够了 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/339207092)
5. [保姆级教程：图解Transformer - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/347904940)
6. [详解Transformer （Attention Is All You Need） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/48508221)
7. https://blog.csdn.net/longxinchen_ml/article/details/86533005
8. [Transformer统治的时代，LSTM模型并没有被代替，LSTM比Tranformer优势在哪里？ - 知乎 (zhihu.com)](https://www.zhihu.com/question/439243827)
9. [关于Transformer的若干问题整理记录 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/82391768)











